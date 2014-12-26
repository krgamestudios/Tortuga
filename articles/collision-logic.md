Abstract:

The goal of this pseudocode is to create a collision system that brings several
colliding bodies to a near-contact state (no space between the bodies, but not
overlapping), while preserving the motion of said bodies. For simplicity, I've
decided to use a square box (BoundingBox, or BBox for short) for the collidable
bodies.

Example:

My current game has a tiled-map, where the tiles are arranged on a 2D grid. The
tiles each have a flag indicating if they are solid (i.e. collidable) or not. I
also have characters walking around on these tile maps, that must not intersect
with solid tiles. For stylistic reasons, I want characters moving at an angle
to "slide" along these walls, and continue along with their original velocity
when they've circumvented the obstacles.

I'd also like to keep the possibility of non-grid collisions using this logic
open (for objects like trees, etc.) if the code allows it.

Obviously, there are many in depth issues that I will need to  take into
account when writing this logic, that have been glossed over or omitted in this
article.

-------------------------

```
	velocity = motion + speed
	if (collision(position + velocity)) then
		if (collision(position + {velocity.x, 0})) then
			velocity.x = 0
		end
		if (collision(position + {0, velocity.y})) then
			velocity.y = 0
		end
	end
	position = position + velocity
```

This code is a basic outline for a collision system that preserves the object's
motion, but it still leaves several pixels of space between the bounding boxes.
Notably, it also treats "collision" as an abstract concept, rather than as an
event that could happen multiple times per frame.

-------------------------

```
	velocity = motion + speed
	if (collisionSimple(BOXSET, position + velocity)) then
		velocity.x = collisionX(BOXSET, velocity.x)
		velocity.y = collisionY(BOXSET, velocity.y)
	end
	position = position + velocity
```

Here, collisions are still abstract, but "BOXSET" is defined externally
(probably as a set of solid boxes, and their positions). This does require more
in depth calculations, as well as three specialized utility functions, but the
results might be what I'm looking for.

If there are any collisions between the player object and the given box set,
then collisionX() and collisionY() are called to calculate the new distance
that the character will move.

-------------------------

```
	bool collisionSimple(BOXSET, newPos):
		for_each box in BOXSET do
			if (box.overlap(PLAYER.box + newPos)) then
				return true
			end
		end
		return false
	end
```

collisionSimple() first runs through the BOXSET, checking if any of the given
bounding boxes would collide with the player object's new position (just
pretend PLAYER is accessible). In this case, non-tile bounding boxes can be
included as part of BOXSET; they're treated just the same. The optimal outcome
is that there are no collisions.

A possible, but flawed, optimization that could be preformed here is to remove
any elements from BOXSET that do not collide with newPos, and let the other
utility functions operate only on what remains. However, if there are any
collisions, than newPos is not the algorithm's final result, therefore any
final result that the algorithm would calculate based on the remaining elements
would not have been checked against the removed elements.

Just something to note.

-------------------------

```
	var collisionX(BOXSET, velocityX):
		var ret = velocityX

		for_each box in BOXSET do
			if (box.overlap(PLAYER.box + PLAYER.position + {velocityX,0})) then
				if (velocityX > 0) then
					ret = min(ret, box.west - PLAYER.position.x)
				else
					ret = max(ret, box.east - PLAYER.position.x)
				end
			end
		end

		return ret
	end
```

Two things: 1. collisionX() and collisionY() should be identical except for the
axis of operation and 2. if a player object is "sliding" along a wall (or
stuck), then these functions will be called every frame.

collisionX() and collisionY() check the sides of the elements in BOXSET, and if
there's a box that the player would collide with, given the current distance to
move, than the distance is reduced, based on if the character is moving left or
right (or up or down).

An unfortunate bug I can already see is that this logic doesn't check corners;
it might be possible to get stuck on a corner of a wall, but if this becomes an
issue in my implementation I will update this article with that information,
and you can promptly ignore it.