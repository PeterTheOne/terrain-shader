terrain-shader
==============

terrain-shader is a project for our shader course at the
University of Applied Sciences Upper Austrian, Campus Hagenberg. 
We want to create a shader for a changing terrain. It is based on the 
tutorials of http://openglbook.com/ and http://www.swiftless.com/

![terrain-shader image](https://raw.github.com/PeterTheOne/terrain-shader/master/result_images/001_terrain-shader_fill-line.png "001 terrain-shader")

Created by:
-----------
- [Peter Grassberger (PeterTheOne)](http://petergrassberger.com)
- [Alexander Hörlesberger (hoerles1983)](http://www.hoerles.com/)

License:
--------
- based on code [MIT Licensed](http://www.opensource.org/licenses/mit-license.php) 
	from http://openglbook.com by [Eddy Luten](https://twitter.com/#!/EddyLuten).

Controlls:
----------
- w, a, s, d	- move camera
- drag mouse	- rotate camera
- l				- switch from line to fill modes
- n, m			- scale terrain

Install - Windows:
------------------
1. update your graphics driver.
2. download and install glut:
	- link: http://www.transmissionzero.co.uk/software/freeglut-devel/
	- download "freeglut 2.6.0-4 for MSVC"
	- put the files in these folders:
		- *.dll					C:\Windows\System32
		- for 64bit systems		C:\Windows\SysWOW64
		- *.lib					C:\Program Files (x86)\Microsoft SDKs\Windows\v7.0A\Lib
		- *.h					C:\Program Files (x86)\Microsoft SDKs\Windows\v7.0A\Include\GL
3. download and install glew:
	- link: http://glew.sourceforge.net/
	- download the "Binaries Windows 32-bit" file
	- make sure use the 32-bit version !!! (also on 64-bit systems)
	- put the files in these folders:
		- *.dll					C:\Windows\System32
		- for 64bit systems		C:\Windows\SysWOW64
		- *.lib					C:\Program Files (x86)\Microsoft SDKs\Windows\v7.0A\Lib
		- *.h					C:\Program Files (x86)\Microsoft SDKs\Windows\v7.0A\Include\GL
4. download and install glext:
	- download: http://www.opengl.org/registry/api/glext.h
	- or google for it.
	- put the files in these folders:
		- glext.h				C:\Program Files (x86)\Microsoft SDKs\Windows\v7.0A\Include\GL
5. download and install devil:
	- link: http://openil.sf.net/
	- download: "DevIL 1.7.8 for 32-bit Windows"
	- make sure use the 32-bit version !!! (also on 64-bit systems)
	- put the files in these folders:
		- *.dll					C:\Windows\System32
		- for 64bit systems		C:\Windows\SysWOW64
		- *.lib					C:\Program Files (x86)\Microsoft SDKs\Windows\v7.0A\Lib
		- *.h					C:\Program Files (x86)\Microsoft SDKs\Windows\v7.0A\Include\IL
6. Done and have fun developing

Install - Windows (Alternative):
------------------------------
You can also put the libary files in the project folder if you want.
*.dll files in the same folder as the *.exe
and the other files in an /lib and /include/GL folder.
You also need to adjust the project settings and 
when commiting make sure to exclude these files.
