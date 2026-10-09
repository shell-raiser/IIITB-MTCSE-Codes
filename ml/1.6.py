import numpy as np

# 1. Women's 100m Olympic Dataset (Year, Winning Time in seconds)
female100 = np.array([
    [1928, 12.20],
    [1932, 11.90],
    [1936, 11.50],
    [1948, 11.90],
    [1952, 11.50],
    [1956, 11.50],
    [1960, 11.00],
    [1964, 11.40],
    [1968, 11.00],
    [1972, 11.07],
    [1976, 11.08],
    [1980, 11.06],
    [1984, 10.97],
    [1988, 10.54],
    [1992, 10.82],
    [1996, 10.94],
    [2000, 11.12],
    [2004, 10.93],
    [2008, 10.78]
])

# 2. Copy the necessary variables (x: Olympic year, t: Winning time)
x = female100[:,[0] ]  # Shape (N, 1)
t = female100[:, [1]]  # Shape (N, 1)

# 3. Augment x with a column of 1s to form design matrix X
# Creates the N x 2 augmented design matrix X by prepending a column of 1s to the attribute vector x. Each row n of X takes the form [ 1  x_n ], allowing the intercept w_0 and slope w_1 to be solved simultaneously via Xw.
ones = np.ones_like(x)
X = np.hstack((ones, x))  # Shape (N, 2)

# 4. Get solution using matrix equation: w = (X^T X)^(-1) X^T t
w = np.linalg.inv(X.T @ X) @ X.T @ t

# Display fitted parameters
w0, w1 = w[0][0], w[1][0]
print(f"Fitted model: t = {w0:.4f} + ({w1:.4f})x")
# print(w)