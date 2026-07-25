# Decisions 

- Prices are considered to be only integer (no floating-point values). This is made to minimize worrying about floating-point calculations and handling because it's not part of goals for this project.

- An array is used to store orders in the order book. We can access elements by prices (that are received from outside) in O(1). It's cache-friendly.

- Potentially, the order generator can generate such an order that the same user (userId) may buy (sell) his/her sell (buy) order. So, userId is the same, price is the same, and order type is opposite. This case is not handled in the order book because it doesn't have to. Such cases have to be rejected way before on user side, if it would be a real application. 