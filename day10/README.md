# Part 1

For part 1 of this problem we first replace the start with the right pipe and
then use its coordinates to remember the start position. Then we pick a
direction to move in the pipe since each pipe has two directions. Once we pick
one, we use the previous and the current pipe to decide the next pipe. We keep
doing this till we get back to the starting coordinates.

# Part 2

We perform the same operations as part 1 but now as we move, we calculate the
area of the polygon using the Trapezoid formula from [Shoelace
formulae](https://en.wikipedia.org/wiki/Shoelace_formula) to calculate area.
Once we calculate the area using the coordinates of the pipes, we can use
[Pick's Theorem](https://en.wikipedia.org/wiki/Pick%27s_theorem) but
substituting the area we calculated and the number of points on the polygon to
calculate the number of points on the inside of the polygon.

We can safely assume that the number of "holes" is 0 since by the problem's
design, it is not possible to have a hole.

