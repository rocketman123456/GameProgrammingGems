Anatomical Human Face Model - Game Programming Gems 8
by Marco Fratarcangeli - 2010
  marco@fratarcangeli.net
  www.fratarcangeli.net


The movies contained in this directory require a player able to show .mov files such as Quicktime, which is 
freely available here: http://www.apple.com/it/quicktime/download/


gpg_method.mov shows the build process of a muscle map from scratch starting from a human skull mesh.  
Muscles are drawn directly on the surface of the skull and then the whole muscoloskeletal system is fit inside the target face.


gpg_results.mov shows the results achievable with the anatomical simulator. There are three sequences:
- a target mesh animated through the anatomical simulation while perform the six basic expressions;

- a target mesh animated while a rigid ball bounces on the cheeks;

- animation of three target faces. Six morph targets of each target face (representic the basic human emotions) have been automatically computed through anatomical simulation and then linearly interpolated to achieve the final animation at a very low computational cost. The rendering in this case is done in 3DS Max, but the anatomical method affects only the animation of the mesh and thus it is independent from the renderer. Any other real-time renderer such as Ogre3D, Irrlicht, etc. can be used as well.





