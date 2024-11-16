# Grafica-Pe-Calculator 💻

University optional course for Computer Graphics in OpenGL, using mainly C++ and GLSL as well as standard libraries for the latest version of OpenGL. As a student using OpenGL at university, I work on simple 2D and 3D projects to learn the fundamentals of graphics programming. This hands-on experience is invaluable for helping me understand essential concepts like the rendering pipeline, transformations, and shaders, which form the backbone of modern computer graphics.

As for the Environment I'm using Microsoft Studio 2022, branding an OpenGL Template Glut, special used for the projects we're developing along each weekly laboratory. Presented below are specific task I've implemented in OpenGL.

<details>
  <summary>
    <h2>Laboratory no. 2 ☄️ </h2> 
  </summary>

  1. Color different primitives using a switch case in the shader and as a plus they should change colors randomly.
     <details>
       <summary>Click to see the results 🌟</summary>
       <div align="center">  
         <img width="100%" alt="image" src="https://github.com/user-attachments/assets/b8e09449-5840-446d-841f-09e5c16c8945">
       </div>
       <div align="center">  
         <img width="100%" alt="image" src="https://github.com/user-attachments/assets/ca7adbb4-c553-42ca-9b94-63fe8117950f">
       </div>
     </details>

  2. Draw a front-facing triangle and a back-facing triangle using different rendering options. 
     For example, the front-facing triangle should be drawn with the FILL option and shaded with a gradient, while the back-facing triangle should use a different option and be colored with a single, uniform color.
     <details>
       <summary>Click to see the results 🌟</summary>
       <div align="center">  
         <img width="100%" alt="image" src="https://github.com/user-attachments/assets/1d8c7bc2-5fda-4d3e-939d-e36f56e0464f">
       </div>
     </details>
</details>

<details>
  <summary>
    <h2>Laboratory no. 3 ☄️</h2>
  </summary>

  1. Choose 𝑎, 𝑏 > 100. Draw two polygons, P1 (convex) and P2 (concave), both positioned within the rectangle [0,a]×[0,b] 
     (meaning the x-coordinates of the vertices are between 0 and a, and the y-coordinates are between 0 and b). 
     For the concave polygon, use an appropriate triangulation for rendering.
     <details>
       <summary>Click to see the results 🌟</summary>
       <div align="center">  
         <img width="100%" alt="image" src="https://github.com/user-attachments/assets/23cabc66-a443-4139-8db3-1c446d825e2d">
       </div>
     </details>

  2. The background should be a gradient.
     <details>
       <summary>Click to see the results 🌟</summary>
       <div align="center">  
         <img width="100%" alt="image" src="https://github.com/user-attachments/assets/de1b21ef-f861-4ed9-aa2f-c19de77b0772">
       </div>
     </details>

  3. Draw both P1 and P2, as well as the figure obtained by applying one of the following transformations: 
     (a) rotation centered "between" P1 and P2, or 
     (b) scaling along arbitrary axes. 
     Specify how you implemented the composition of transformations – either by multiplying the matrices in the shader or by performing matrix multiplication in the main program.
     <details>
       <summary>Click to see the results 🌟</summary>
       <div align="center">  
         <img width="100%" alt="image" src="https://github.com/user-attachments/assets/0134f547-c6c8-4bbf-8040-5738c45eaba4">
       </div>
     </details>
</details>

<details>
  <summary>
    <h2>Laboratory no. 4 ☄️</h2>
  </summary>

  1. Using `glDrawElements()` with indexed vertices, draw the figure below.
     <details>
       <summary>Click to see the results 🌟</summary>
       <div align="center">  
         <img width="100%" alt="image" src="https://github.com/user-attachments/assets/f3479dae-9820-465b-8fcb-f9259b1fcdbd">
       </div>
     </details>

  1.1. Generate 𝑛 points (where 𝑛=8,10,12—choose a value) on two concentric circles centered at the origin, 
       and then draw an outline similar to the one below (in this example 𝑛=8; adapt the outline based on your chosen value). 
       - **Hint**: Points on a circle with center 𝑂 and radius 𝑟 can be represented as:
         - 𝑥 = 𝑟 cos(𝜃)  
         - 𝑦 = 𝑟 sin(𝜃)  
         where 𝜃 is in [0, 2𝜋). Select 𝜃 values in the form `2𝑘𝜋/𝑛`, with 𝑘 = 0, 1, 2, …, 𝑛−1 to obtain 𝑛 points on the circle (forming a regular 𝑛-sided polygon).
     <details>
       <summary>Click to see the results 🌟</summary>
         <div align="center">  
           <img width="100%" alt="image" src="https://github.com/user-attachments/assets/a4b3f63d-269f-44c0-bbed-5c3350983927">
         </div>
     </details>

  2. Create a simple drawing (e.g., a house and tree) using two VAOs or two VBOs.
     <details>
       <summary>Click to see the results 🌟</summary>
       <div align="center">  
         <img width="100%" alt="image" src="https://github.com/user-attachments/assets/acf3e88f-44bc-41ef-b04a-a66e03f08125">
       </div>
     </details>

  3. In the initial source code which currently only draws a square:
     - Add matrices for a scaling transformation with factors `(2.0, 0.5)` and a translation vector `(100, 100)`. 
     - Be sure to update the shader as needed. Then, draw both of the following in the same scene:
       - **Object 1**: A color blend object, created by applying the scaling first and then the translation.
       - **Object 2**: A textured object, created by applying the translation first and then the scaling.
     <details>
       <summary>Click to see the results 🌟</summary>
       <div align="center">  
         <img width="100%" alt="image" src="https://github.com/user-attachments/assets/38845da6-60db-42c4-b896-cab9dd69a53b">
       </div>
     </details>
</details>

<details>
  <summary>
    <h2>2D PROJECT 🚔</h2>
  </summary>
      <details>
        <summary><h3>Description</h3></summary>
          The project simulates a traffic scene in which a car 🚗 attempts to safely overtake another vehicle on a road, using OpenGL graphics. The program visually represents an animated road with multiple moving cars. Each car has its own characteristics for positioning, speed, and behavior, while the central action of the simulation focuses on the overtaking attempt of a vehicle ahead.<br>
          The program manages animations, collisions 💥, and the synchronization of car movements, creating a realistic traffic atmosphere. Additionally, the road and background provide visual context, while textures and rotations enhance the realism of the scene.<br>
          To add complexity to the project, the overtaking maneuver is triggered by the user, via a mouse 🖱️ or touchpad click 🌠. As an additional element, the simulation includes an oncoming car driving in the opposite lane. <br>
          If the user’s action leads to a collision 💥 with the oncoming car during the overtaking attempt, an accident occurs, causing the oncoming car to be thrown off the road. Subsequently, a police car 🚓 appears and stops at the scene of the accident. Conversely, if the overtaking maneuver is completed successfully, all cars continue their respective trajectories. <br>
      </details>
</details>
