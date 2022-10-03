# Obj2Geom
A tool to convert a wavefront OBJ file to a basic GEOM for MGS:V.

## Usage
Simply drag and drop a wavefront OBJ file to **Obj2Geom.exe** and it should generate a GEOM file (making sure that the orientations of the OBJ fit with MGS:V's, see the troubleshooting section). However, there's a limitation of a maximum 255 faces for the OBJ file. The tool also allows you to change the material type and the collision tags (what it should collide with, for instance physics items, the player etc) of the generated GEOM, see below for the steps.

## Mesh Recommendations
GEOM files use quads, so it's highly recommended that your OBJ mesh contains as many of them as possible. But ***be aware*** that the game's collision algorithm can't handle concave or non-planar quads, so only basic flat shapes will work correctly. If your mesh must use non-planar quads, consider converting them to triangles (and the tool will automatically make them into degenerate quads). 3D modeling software have an automatic way of turning non-planar quads to triangles. For Blender it's done like so, make sure you've selected your mesh and went into edit mode:

![Splitting non-planar faces in Blender](https://i.imgur.com/zUTRfuQ.png)
There's also an option for the max angle that will appear on the bottom left of the screen. The default's at **5** degrees, but from my testing **3.4** appears to be the sweet spot.

![Max angle property](https://i.imgur.com/6453IDC.png)

This process would probably be similar for 3DS Max and other 3D modeling software.

## Changing the material type
For this you'll have to use the command line, and run **Obj2Geom.exe** with the ***first(!)*** parameter being `-mtr <index>` and the second would be the path to your OBJ file. `<index>` is a number from 0 to 81, since there are 82 material types in MGS:V. To get a list of the material types and their index, double click on **Obj2Geom.exe**.
Here's a visual example:

![Visual example](https://i.imgur.com/QMQMdCB.gif)

The `-mtr` parameter can also be chained with other files. For example, running the command
```bat
Obj2Geom.exe -mtr 10 example.obj -mtr 20 example2.obj -mtr 30 example3.obj
```
will create **`example.geom`** with material type index 10, **`example2.geom`** with material type index 20 and **`example3.geom`** with material type index 30.

Also note that if no `-mtr` parameter is specified, Obj2Geom will automatically use `MTR_IRON_A` as a material type, so for the following command **`example3.geom`** will use material type index 30 and the others material type index 0, which corresponds to `MTR_IRON_A`:
```bat
Obj2Geom.exe example.obj example2.obj -mtr 30 example3.obj
```

## Changing the collision tags
This is achieved via the **collision_flags.txt** file, since it would be way too cumbersome to do it through command arguments. Simply add or remove the lines that correspond to a specific tag (**PLAYER**, **ENEMY** etc). Comments start with ***'#'*** and are ignored by the program. After updating the text file, the changes will be applied the next time you generate a GEOM file. A list of tags is contained within this file as comments.

## Troubleshooting
If collision doesn't work at all in-game, there's 2 things that might have gone wrong: you didn't add the proper collision tags (the default ones will work), or the mesh's orientation is wrong compared to MGS:V's orientations. For Blender users, always export your OBJ files with these two settings, something similar would have to be done for other 3D modeling software:

![Export settings for OBJ files](https://i.imgur.com/WU1osnD.png)

Regarding the collision tags, Obj2Geom will automatically use the most useful tags if **collision_flags.txt** happens to be missing (you'll also see a warning if you use the tool through the command line).