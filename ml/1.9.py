#solves Exercise 1.9 by fitting polynomial regression models and performing 10-fold cross-validation to select the optimal polynomial order
import numpy as np
import matplotlib.pyplot as plt
import scipy.io

# 1. Load data from synthdata.mat
data = scipy.io.loadmat('synthdata.mat')
x = data['x'].reshape(-1, 1)  # Shape (N, 1)
t = data['t'].reshape(-1, 1)  # Shape (N, 1)
N = len(x)

# --- Part 1: Fit 4th-order polynomial ---
X_poly4 = np.hstack([x**k for k in range(5)])  # [1, x, x^2, x^3, x^4]
w_4th = np.linalg.inv(X_poly4.T @ X_poly4) @ X_poly4.T @ t # Computes the least squares parameter weights $\hat{\mathbf{w}} = (\mathbf{X}^T \mathbf{X})^{-1} \mathbf{X}^T \mathbf{t}$

print("Fitted weights for 4th-order polynomial:")
for i, w_val in enumerate(w_4th.ravel()):
    print(f"  w_{i} = {w_val:.4f}")

# --- Part 2: Set up 10-fold cross-validation boundaries ---
np.random.seed(42)  # For reproducible shuffling
order = np.random.permutation(N)

k_folds = 10
sizes = np.full(k_folds, N // k_folds)
sizes[-1] += N - np.sum(sizes)  # Adjust last fold for remainder
boundaries = np.insert(np.cumsum(sizes), 0, 0)

# Matrix to store squared losses: shape (4 orders, 10 folds)
loss = np.zeros((4, k_folds))

# --- Part 3: 10-Fold Cross-Validation Loop ---
X_cv = np.ones((N, 1))  # Start with intercept column (x^0)

for poly_order in range(1, 5):
    # Incrementally add power x^poly_order
    X_cv = np.hstack((X_cv, x**poly_order))
    
    for k in range(k_folds):
        # Identify indices for test fold vs training folds
        test_idx = order[boundaries[k]:boundaries[k+1]]
        train_idx = np.delete(order, np.arange(boundaries[k], boundaries[k+1]))
        
        # Split train and test sets
        X_train, t_train = X_cv[train_idx], t[train_idx]
        X_test, t_test = X_cv[test_idx], t[test_idx]
        
        # Fit model on training folds
        w_fit = np.linalg.inv(X_train.T @ X_train) @ X_train.T @ t_train
        
        # Compute predictions and test loss on holdout fold
        predictions = X_test @ w_fit
        loss[poly_order - 1, k] = np.sum((predictions - t_test)**2)

# --- Part 4: Average and Plot Loss ---
mean_loss = np.mean(loss, axis=1)

plt.figure(figsize=(7, 5))
plt.plot(range(1, 5), mean_loss, 'bo-', linewidth=2, markersize=8)
plt.xlabel('Polynomial Order')
plt.ylabel('Mean Cross-Validation Loss (SSE)')
plt.title('10-Fold Cross-Validation for Polynomial Order Selection')
plt.xticks(range(1, 5))
plt.grid(True)
plt.show()
