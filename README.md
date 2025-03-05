# BoidsSimulation

CONTROLS:

- WASD = Horizontal Movement 
- Space / Left Control = Vertical Movement
- Left Shift = Dash in Movement Direction
- Left Mouse Button = Spawns Predator Projectile ( Goes Towards nearest boid)
- Right Mouse Button = Spawns Attractor Projectile ( Attracts Boids)
- O / P = Despawn / Spawn more Boids
- K / L = Decrement / Increment steering update interval

![me](https://github.com/VeryHotShark/BoidsSimulation/blob/main/BoidsGif.gif)

Longer Video + Stress Test : https://youtu.be/DnRZYCYpyOw

Hello this is a boid simulation I wrote in c++ as a test for one company while ago. 
The project is written on top of the simple Framework I was provided in which basic camera movement was Implemented and loading the Skyscrapers meshes (simble box shapes).
Due to copyright I can't share the framework and can only share the parts of the Code I wrote, so there is no project solution to check.

What I implemented was :
- Ability to shoot Projectile and for them to physically react with boxes 
- Boids Simulation, Avoiding Each Other and Skyscrapers

Most important scripts to check :
- [SpatialHashGrid](https://github.com/VeryHotShark/BoidsSimulation/blob/main/Sources/SpatialHashGrid.h)
- [BoidManager](https://github.com/VeryHotShark/BoidsSimulation/blob/main/Sources/BoidManager.cpp)
- [BoidSteeringController](https://github.com/VeryHotShark/BoidsSimulation/blob/main/Sources/BoidSteeringController.cpp)
- [ProjectileController](https://github.com/VeryHotShark/BoidsSimulation/blob/main/Sources/ProjectileController.cpp)
- [Projectile](https://github.com/VeryHotShark/BoidsSimulation/blob/main/Sources/Projectile.cpp)

For birds flocks I used simple algorithm from Craig Reynolds using Separation, Cohesion and Alignment.
For collisions I mostly used AABB (Axis Aligned Bounding Box) + simple Radius Checks
For physics like Projectile movement I used some basic valocity, acceleration, drag calculations.

The biggest Challenge I faced was Optimization for the large amounts of Boids and how they detect each other.
I experimented with Octrees at first but the boids are to dynamic, so recreating/updating the Tree was too expensive
Instead I went with simple SpatialHashGrid implementation to speed up the Query process using the Cell Index.

I thought a lot about architecture, how to structure things. I tried to have things as decoupled as I could, have some base structures to avoid repetition (Entity, Bounds struct)
I wanted to create some base SteeringBehavior class with GetSteeringMethod and weight multiplier but that would require virtual function and I wanted to avoid that. 
Since each boid has the same behavior I decided to go with SteeringController to manage all kinds of steering. If there were different kinds of boids then I would seperate behaviors so we can mix and match pieces we want.

When it comes to communication between classes, I used Game class as a mediator holding References to other classes, and I would pass the reference of Game instance to classes that needs it. (ex. BoidsManager fetches Skyscrapers from Game instance)
One thing I'm not proud of is the Delegate Binding to Projectile OnDestroy in BoidManager, because it tight couples them, so I should probably refactor that.

I was thinking about ownership and whether SpatialHashGrid should store shared ptr/weak pointers, but decided to go for raw pointers, so the user of SpatialHashGrid has full control over lifetime and ownership of objects.
I also Implemented simple cooldown for steering behavior update which is set to 0.0f at by default to have most accurate steering, but this helps optimisation since we donnt need to update new steering force every frame.

Lastly I added some simple smoothing to movement/look and HUD
In general there is a lot of space for improvements, but I got stuck on Octrees fail / SpatialHashGrid so I run out of time. 
For example Projectiles could also be in seperate HashGrid for Boids to query if there would be more of them, Skyscrapers could be placed in Octree so Boids query for only ones that are in the same/neighboring cells.
I could also create some simple InputWrapper since I only used OnKeyReleased event in this project. better Event Handling Mechanism instead of Direct Binding. And better physics collision detection since there are some cases where it acts weird.
