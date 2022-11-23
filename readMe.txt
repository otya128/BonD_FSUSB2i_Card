-----BEGIN PGP SIGNED MESSAGE-----
Hash: SHA256

# BonDriver_FSUSB2i.dll
2016-02-19, Ver. 0.1.5

This is a BonDriver DLL for FSUSB2i (ISDB-T TV Tuner)
C, Visual C++ source code package (MS Visual C++ 2010)



## Files

/
	readMe.txt
	ChangeLog.txt
	sha1sum.txt
	sha1sum.txt.sig
		* gpg --verify sha1sum.txt.sig

src/
	* source code files for Windows programs
	FSUSB2i.sln
		* VS solution file
	it9175.c
		* Device operation code (same as Linux ver.)
	*.{c,cpp,h}

src/BonDriver/
	* source code for BonDriver
	BonDriver_FSUSB2i.vcxproj{,.*}
		* VS project file
	*.{c,cpp,h}
	BonDriver.rc



## User Agreement

USE AT YOUR OWN RISK, no warranty

You can ...
* use this package for any purposes.
* redistribute a copy of this package without modifying.
* modify this package.
* redistribute the modified package. You should write the modification clearly.

based on GPLv3



## How to compile and build?

I use the VC++ Express version. (Microsoft Visual C++ 2010 SP1)
You can download from MS site.

Build the "BonDriver_FSUSB2i" project.

(x64 environment setting has NOT been checked YET.)



## End

(c) 2015-2016 trinity19683
signed by "trinity19683.gpg-pub" 2015-12-13
-----BEGIN PGP SIGNATURE-----
Version: GnuPG v1

iQEcBAEBCAAGBQJWyBh3AAoJEB0tpC02lUUYMeQH/R7dPgyvuYAAh/xwj3rzzHPw
dLJr6o+xYChLvvdH1YQUN4/fLeRAUg3T9caK0IPEfmc677+SrV8hRML5s13Lbawv
4urFfme488tcopc2QMeDSUappW0hAEuXL2AwT5KbpX8Pyo5Cqm9zyRe9Q046yziV
/0ObsQeqXCjyAUm0Kjx0rkeWM3Yb1g7iQDwQMFyI2mZLTq1erTsTnPHYZsfB7kEU
FUWYJNgBiHKvpoy+g/2do5/X6vCgCVvueKpf7pstu9+M0UyDswpR7deRUUE46AUC
1ArjU42erJPSBq2ZqbIuOqgxpiN+mOHQCjP/NvIReZevfqOZLxnMibMdiK0r1NQ=
=tG5L
-----END PGP SIGNATURE-----
