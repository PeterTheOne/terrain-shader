terrain-shader
==============

terrain-shader is a project for our shader course at the
University of Applied Sciences Upper Austrian, Campus Hagenberg. 
We want to create a shader for a changing terrain. It is based on these 
tutorials:
- [OpenGL Book](http://openglbook.com/) - nice start for OpenGL 3.3 and 4.0 learning
- [Switfless Tutorials](http://www.swiftless.com/) - read, compile and link GLSL shader
- [wikibooks: OpenGL_Programming](http://en.wikibooks.org/wiki/OpenGL_Programming)
	- [Glescraft_4](http://en.wikibooks.org/wiki/OpenGL_Programming/Glescraft_4) - first person camera controls

![terrain-shader image](https://raw.github.com/PeterTheOne/terrain-shader/master/result_images/001_terrain-shader_fill-line.png "001 terrain-shader")

Created by:
-----------
- [Peter Grassberger (PeterTheOne)](http://petergrassberger.com)
- [Alexander Hörlesberger (hoerles1983)](http://www.hoerles.com)

Special thanks to:
------------------
- [prettyvanilla](https://bitbucket.org/prettyvanilla) for some nice links.
- [Stefan Grassberger (StevyGee)](http://stefangrassberger.com) for keeping me going.

License:
--------
- based on [MIT Licensed](http://www.opensource.org/licenses/mit-license.php) code 
	from http://openglbook.com by [Eddy Luten](https://twitter.com/#!/EddyLuten).

Controlls:
----------
- escape		- exit
- w, a, s, d	- move camera
- drag mouse	- rotate camera
- l				- switch from line to fill modes
- n, m			- scale terrain

Install - Windows:
------------------
1. update your graphics driver.
2. download: 
	- freeglut: 
		- link: http://www.transmissionzero.co.uk/software/freeglut-devel/
		- file: "freeglut 2.6.0-4 for MSVC"
	- glew: 
		- link: http://glew.sourceforge.net/
		- file: "Binaries Windows 32-bit"
		- make sure use the 32-bit version! (also on 64-bit systems)
	- devil: 
		- link: http://openil.sf.net/
		- file: "DevIL 1.7.8 SDK for 32-bit Windows"
		- make sure use the 32-bit version! (also on 64-bit systems)
	- glm
		- link: http://glm.g-truc.net/
		- file: "GLM 0.9.3.B"
3. copy files in windows directories (only 32-bit files): 
	- put the files in these windows folders: 
		- *.dll					C:\Windows\System32
		- for 64bit systems		C:\Windows\SysWOW64
		- *.lib					C:\Program Files (x86)\Microsoft SDKs\Windows\v7.0A\Lib
		- *.h					C:\Program Files (x86)\Microsoft SDKs\Windows\v7.0A\Include\GL or IL or GLM
	- or in these project folders: 
		- *.dll					\
		- *.lib					\lib\
		- *.h					\include\GL or IL or GLM
4. Done and have fun developing
