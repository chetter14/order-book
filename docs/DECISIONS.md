# Decisions 

- Prices are considered to be only integer (no floating-point values). This is made to minimize worrying about floating-point calculations and handling because it's not part of goals for this project.

- An array is used to store orders in the order book. We can access elements by prices (that are received from outside) in O(1). It's cache-friendly.

- 