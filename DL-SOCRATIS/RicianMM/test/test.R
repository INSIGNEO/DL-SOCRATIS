library(VGAM)

# NOT RUN {
 x <- seq(0.01, 7, len = 201)
plot(x, drice(x, vee = 0, sigma = 1), type = "n", las = 1,, ylab = "",
     main = "Density of Rice Gaussian mixture model distributions")
sigma <- 1; vee <- c(0, 3)
for (ii in 1:length(vee))
  lines(x, drice(x, vee = vee[ii], sigma), col = ii)
legend(x = 5, y = 0.6, legend = as.character(vee),
       col = 1:length(vee), lty = 1)

x <- seq(0, 4, by = 0.01); vee <- 1; sigma <- 1
probs <- seq(0.05, 0.95, by = 0.05)
plot(x, drice(x, vee = vee, sigma = sigma), type = "l", col = "blue",
     main = "Blue is density, orange is cumulative distribution function",
     ylim = c(0, 1), sub = "Purple are 5, 10, ..., 95 percentiles",
     las = 1, ylab = "", cex.main = 0.9)
abline(h = 0:1, col = "black", lty = 2)
Q <- qrice(probs, sigma, vee = vee)
lines(Q, drice(qrice(probs, sigma, vee = vee),
               sigma, vee = vee), col = "purple", lty = 3, type = "h")
lines(x, price(x, sigma, vee = vee), type = "l", col = "orange")
lines(Q, drice(Q, sigma, vee = vee), col = "purple", lty = 3, type = "h")
lines(Q, price(Q, sigma, vee = vee), col = "purple", lty = 3, type = "h")
abline(h = probs, col = "purple", lty = 3)
max(abs(price(Q, sigma, vee = vee) - probs))  # Should be 0
# }
#print random rice values.
print(rrice(100,sigma=30,vee=100))

#run the file by Rscript file_name.R
