# DL-SOCRATIS

The DL-SOCRATIS pipeline builds on MA-SOCRATIS  by modifying the initial myocardium segmentation step. Instead of a multi-atlas segmentation framework, in DL-SOCRATIS we use two different deep learning networks approaches. A combination of a convolution deep neural network for left ventricle detection, and an u-net LGE-MRI trained network for the myocardium segmentation (DLl-SOCRATIS). For the second approach we used a cycle-GAN network to translate the LGE-MR images to cine-MRI and segment the myocardium using an u-net network trained in cine-MR images (DLc-SOCRATIS).
After the initial myocardium segmentation, MA-SOCRATIS uses a combination of thresholding and active-contour segmentation  algorithms for the epicardium and endocardium final contour segmentation. The automatic segmentation scar region technique is identical to that used in MA-SOCRATIS. It utilises a combination of Rician-Gaussian mixture models histogram fitting (RGMM) and watershed segmentation constrained by seed points.

If you use the software please cite the follow paper:
