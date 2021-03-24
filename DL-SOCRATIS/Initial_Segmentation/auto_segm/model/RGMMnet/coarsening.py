import numpy as np
import scipy.sparse

from auto_segm.model.RGMMnet import utils2, graph

def grid_graph(m):
  number_edges=8
  metric ='euclidean'
  z = graph.grid(m)  # normalized nodes coordinates
  dist, idx = graph.distance_sklearn_metrics(z, k=number_edges, metric=metric) 
  A = graph.adjacency(dist, idx) 

  return A, z


def coarsen_mnist(A, levels, nodes_coordinates):
    graphs, parents = metis(A, levels) 
    perms = compute_perm(parents) 
    coordinates = np.copy(nodes_coordinates)
    u_shape, u_rows, u_cols, u_val = [], [], [], []
    
    for i,A in enumerate(graphs):
        M, M = A.shape

        # We remove self-connections created by metis.
        A = A.tocoo()
        A.setdiag(0)

        if i < levels: #if we have to pool the graph 
            A = perm_adjacency(A, perms[i]) 
        A = A.tocsr()
        A.eliminate_zeros()
        
        Mnew, Mnew = A.shape
        u_shape.append(Mnew)
        
        if i == 0:
            fake_nodes = Mnew - M
            coordinates = np.concatenate([coordinates, np.ones([fake_nodes, 2])*np.inf], 0)
            coordinates = coordinates[perms[0]]
        
        start_node, end_node = A.nonzero()
        u_rows.append(start_node)
        u_cols.append(end_node)
        
        distance = coordinates[start_node] - coordinates[end_node]
        u_val.append(distance)
        
        print('Layer {0}: M_{0} = |V| = {1} nodes ({2} added), |E| = {3} edges'.format(i, Mnew, Mnew-M, A.nnz//2))
        
        # update coordinates for next coarser graph
        new_coordinates = []
        for k in range(A.shape[0]//2):
            idx_first_el = k * 2
            
            if not np.isfinite(coordinates[idx_first_el][0]):
                new_coordinates.append(coordinates[idx_first_el+1])
                
            elif not np.isfinite(coordinates[idx_first_el+1][0]):
                new_coordinates.append(coordinates[idx_first_el])
                
            else:
                new_coordinates.append(np.mean(coordinates[idx_first_el:idx_first_el+2], axis=0))
                
        coordinates = np.asarray(new_coordinates)

    return u_shape, u_rows, u_cols, u_val, perms[0]



def metis(W, levels, rid=None):
    """
    Coarsen a graph multiple times using the METIS algorithm.

    INPUT
    W: symmetric sparse weight (adjacency) matrix
    levels: the number of coarsened graphs

    OUTPUT
    graph[0]: original graph of size N_1
    graph[2]: coarser graph of size N_2 < N_1
    graph[levels]: coarsest graph of Size N_levels < ... < N_2 < N_1
    parents[i] is a vector of size N_i with entries ranging from 1 to N_{i+1}
        which indicate the parents in the coarser graph[i+1]
    nd_sz{i} is a vector of size N_i that contains the size of the supernode in the graph{i}

    NOTE
    if "graph" is a list of length k, then "parents" will be a list of length k-1
    """

    N, N = W.shape
    if rid is None:
        rid = np.random.permutation(range(N))
    parents = []  # contains in each position the id of the clusters where two nodes have been placed
    degree = W.sum(axis=0) - W.diagonal()
    graphs = []
    graphs.append(W)
    
    for _ in range(levels):
        # cluster the provided graph #levels times
        
        # CHOOSE THE WEIGHTS FOR THE PAIRING
        # weights = ones(N,1)       # metis weights
        weights = degree            # graclus weights
        # weights = supernode_size  # other possibility
        weights = np.array(weights).squeeze()

        # PAIR THE VERTICES AND CONSTRUCT THE ROOT VECTOR
        idx_row, idx_col, val = scipy.sparse.find(W)
        perm = np.argsort(idx_row)
        rr = idx_row[perm]
        cc = idx_col[perm]
        vv = val[perm]
        cluster_id = metis_one_level(rr,cc,vv,rid,weights)  # rr is ordered
        parents.append(cluster_id)

        # COMPUTE THE EDGES WEIGHTS FOR THE NEW GRAPH
        nrr = cluster_id[rr]  # nrr and ncc are the idx of edges after clustering, self-loops are produced for each node by these new edges
        ncc = cluster_id[cc]
        nvv = vv
        Nnew = cluster_id.max() + 1
        
        # CSR is more appropriate: row,val pairs appear multiple times
        W = scipy.sparse.csr_matrix((nvv,(nrr,ncc)), shape=(Nnew,Nnew))
        W.eliminate_zeros()
        # Add new graph to the list of all coarsened graphs
        graphs.append(W)
        N, N = W.shape

        # COMPUTE THE DEGREE (OMIT OR NOT SELF LOOPS)
        degree = W.sum(axis=0)
        
        # CHOOSE THE ORDER IN WHICH VERTICES WILL BE VISTED AT THE NEXT PASS
        #[~, rid]=sort(ss);     # arthur strategy
        #[~, rid]=sort(supernode_size);    #  thomas strategy
        #rid=randperm(N);                  #  metis/graclus strategy
        ss = np.array(W.sum(axis=0)).squeeze()
        rid = np.argsort(ss)

    return graphs, parents


# Coarsen a graph given by rr,cc,vv.  rr is assumed to be ordered
def metis_one_level(rr,cc,vv,rid,weights):

    nnz = rr.shape[0]
    N = rr[nnz-1] + 1

    marked = np.zeros(N, np.bool)   # identifies already processed nodes
    rowstart = np.zeros(N, np.int32)  # contains the idx of the edges where a new row start
    rowlength = np.zeros(N, np.int32)  # contains in every entry the number of edges associated to the row
    cluster_id = np.zeros(N, np.int32)  # contains the idx of the clusters of nodes after pairing

    oldval = rr[0]
    count = 0
    clustercount = 0

    for ii in range(nnz):
        rowlength[count] = rowlength[count] + 1
        if rr[ii] > oldval:
            oldval = rr[ii]
            rowstart[count+1] = ii
            count = count + 1

    for ii in range(N):
        tid = rid[ii]  # take the ii-th node in the graph randomly sorted
        if not marked[tid]:
            wmax = 0.0
            rs = rowstart[tid]
            marked[tid] = True
            bestneighbor = -1
            for jj in range(rowlength[tid]):  # iterating on the edges associated node
                # it finds the closest neighbor to node tid, namely the one with highest w_ij * 1/(\sum_k w_ik) + w_ij * 1/(\sum_k w_kj) 
                # basically we weight each edge by the sum of the propabilities that a random walker would work from i to j and from j to i 
                # (we favour strong connections, i.e. the ones with high weight wrt all the others for both nodes, in the choice of the neighbor of node tid)
                nid = cc[rs+jj]  
                if marked[nid]:
                    tval = 0.0
                else:
                    tval = vv[rs+jj] * (1.0/weights[tid] + 1.0/weights[nid])  # w_ij * 1/(\sum_k w_ik) + w_ij * 1/(\sum_k w_kj) 
                if tval > wmax:
                    wmax = tval
                    bestneighbor = nid

            cluster_id[tid] = clustercount

            if bestneighbor > -1:
                cluster_id[bestneighbor] = clustercount
                marked[bestneighbor] = True

            clustercount += 1

    return cluster_id

def compute_perm(parents):
    """
    Return a list of indices to reorder the adjacency and data matrices so
    that the union of two neighbors from layer to layer forms a binary tree.
    It adds at level l, all the fake nodes associated to l and all the additional 
    ones generated by next coarsening layers (needed to compute the entire 
    coarsening via max-pooling).
    """

    # Order of last layer is random (chosen by the clustering algorithm).
    indices = []
    if len(parents) > 0:
        M_last = max(parents[-1]) + 1
        indices.append(list(range(M_last)))

    for parent in parents[::-1]:
        #print('parent: {}'.format(parent))

        # Fake nodes go after real ones.
        pool_singeltons = len(parent)

        indices_layer = []
        for i in indices[-1]:
            indices_node = list(np.where(parent == i)[0])
            assert 0 <= len(indices_node) <= 2
            
            # Add a node to go with a singelton.
            if len(indices_node) is 1:
                indices_node.append(pool_singeltons)
                pool_singeltons += 1
            # Add two nodes as children of a singelton in the parent.
            elif len(indices_node) is 0:
                indices_node.append(pool_singeltons+0)
                indices_node.append(pool_singeltons+1)
                pool_singeltons += 2
                #print('singelton childrens: {}'.format(indices_node))

            indices_layer.extend(indices_node)
        indices.append(indices_layer)

    # Sanity checks.
    for i,indices_layer in enumerate(indices):
        M = M_last*2**i
        # Reduction by 2 at each layer (binary tree).
        assert len(indices[0] == M)
        # The new ordering does not omit an indice.
        assert sorted(indices_layer) == list(range(M))

    return indices[::-1]

assert (compute_perm([np.array([4,1,1,2,2,3,0,0,3]),np.array([2,1,0,1,0])])
        == [[3,4,0,9,1,2,5,8,6,7,10,11],[2,4,1,3,0,5],[0,1,2]])

def perm_data(x, indices):
    """
    Permute data matrix, i.e. exchange node ids,
    so that binary unions form the clustering tree.
    """
    if indices is None:
        return x

    N, M = x.shape
    Mnew = len(indices)
    assert Mnew >= M
    xnew = np.empty((N, Mnew))
    for i,j in enumerate(indices):
        # Existing vertex, i.e. real data.
        if j < M:
            xnew[:,i] = x[:,j]
        # Fake vertex because of singeltons.
        # They will stay 0 so that max pooling chooses the singelton.
        # Or -infty ?
        else:
            xnew[:,i] = np.zeros(N)
    return xnew

def perm_adjacency(A, indices):
    """
    Permute adjacency matrix, i.e. exchange node ids,
    so that binary unions form the clustering tree.
    """
    if indices is None:
        return A

    M, M = A.shape
    Mnew = len(indices)
    assert Mnew >= M
    A = A.tocoo()

    # Add Mnew - M isolated vertices.
    if Mnew > M:
        rows = scipy.sparse.coo_matrix((Mnew-M,    M), dtype=np.float32)
        cols = scipy.sparse.coo_matrix((Mnew, Mnew-M), dtype=np.float32)
        A = scipy.sparse.vstack([A, rows])
        A = scipy.sparse.hstack([A, cols])

    # Permute the rows and the columns.
    perm = np.argsort(indices)  # perm contains in poistion i where node i is placed according to permutation "indices"
    A.row = np.array(perm)[A.row]
    A.col = np.array(perm)[A.col]

    # assert np.abs(A - A.T).mean() < 1e-9
    assert type(A) is scipy.sparse.coo.coo_matrix
    return A

def convert_train_data(x_init,perm,im_length):
  x_init=np.array(x_init)
  print(x_init.shape)
  x_train_original=x_init.reshape(-1,im_length*im_length)
  x_train = coarsening.perm_data(x_train_original, perm)

  x_train = np.expand_dims(x_train, -1)

  print(x_train.shape)
  return x_train
