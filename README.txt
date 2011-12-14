terrain-shader

terrain-shader is a project for our shader course at the
University of Applied Sciences Upper Austrian, Campus Hagenberg. 
We want to create a shader for changing world.

Created by:
Peter Grassberger (PeterTheOne)
Alexander Hörlesberger (hoerles1983)

Install - Windows:

1. download and install glut:
	- link: http://user.xmission.com/~nate/glut.html
	- download the glut-3.7.6-bin.zip (117 KB) file
	- put the files in these folders:
		- glut32.dll			C:\Windows\System32
		- for 64bit systems		C:\Windows\SysWOW64
		- glut32.lib			C:\Program Files (x86)\Microsoft SDKs\Windows\v7.0A\Lib
		- glut.h				C:\Program Files (x86)\Microsoft SDKs\Windows\v7.0A\Include\GL
2. download and install glew
	- link: http://glew.sourceforge.net/
	- download the "Binaries Windows 32-bit" file
	- make sure use the 32-bit version !!! (also on 64-bit systems)
	- put the files in these folders:
		- *.dll					C:\Windows\System32
		- for 64bit systems		C:\Windows\SysWOW64
		- *.lib					C:\Program Files (x86)\Microsoft SDKs\Windows\v7.0A\Lib
		- *.h					C:\Program Files (x86)\Microsoft SDKs\Windows\v7.0A\Include\GL
3. download and install glext
	- download: http://www.opengl.org/registry/api/glext.h
	- or google for it.
	- put the files in these folders:
		- glext.h				C:\Program Files (x86)\Microsoft SDKs\Windows\v7.0A\Include\GL

4. Done and have fun developing

Install - Windows Alternative:

You can also put the libary files in the project folder if you want.

*.dll files in the same folder as the *.exe
and the other files in an /lib and /include/GL folder.
You also need to adjust the project settings and 
when commiting make sure to exclude these files.

huhu ois foisch