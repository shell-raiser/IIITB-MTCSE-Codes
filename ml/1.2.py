# Write a ~MATLAB~ python script that can find w0 and w1 for an arbitrary dataset of xn, tn pairs.
# This uses the least squares approach
import numpy as np
import matplotlib.pyplot as plt

# 1. Sample dataset: each x value must have a matching t value.
x = np.array([13, 14, 15], dtype=float)
t = np.array([4.8, 11.3, 17.2], dtype=float)

# 2. Compute averages: x-bar, t-bar, mean(x^2), and mean(x*t).
xb = np.mean(x)         # Mean of x (x-bar)
tb = np.mean(t)         # Mean of t (t-bar)
x2b = np.mean(x * x)    # Mean of x^2 (element-wise multiplication)
xtb = np.mean(x * t)    # Mean of x*t (element-wise multiplication)

# 3. Least-squares theory: w1 is the slope and w0 is the intercept.
# The fitted line is t_hat = w0 + w1*x.
w1 = (xtb - tb * xb) / (x2b - xb**2) # $$\hat{w}_1 = \frac{\overline{xt} - \bar{x}\bar{t}}{\overline{x^2} - (\bar{x})^2}$$
w0 = tb - w1 * xb # $$\hat{w}_0 = \bar{t} - w_1 \bar{x}$$

print(f"w0 (intercept): {w0:.4f}")
print(f"w1 (slope):     {w1:.4f}")

# 4. Plot the data and the linear model
plt.figure(figsize=(7, 5))
plt.plot(x, t, 'b.', markersize=15, label='Data points')
plt.plot(x, w0 + w1 * x, 'r-', linewidth=2, label=f'Model: t = {w0:.2f} + {w1:.2f}x')

plt.xlabel('Attribute (x)')
plt.ylabel('Target (t)')
plt.title('Least Squares Linear Model Fit')
plt.legend()
plt.grid(True)
plt.show()
