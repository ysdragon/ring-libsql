load "stdlibcore.ring"

cPathSep = "/"

if isWindows()
	cPathSep = "\\"
ok

# Remove the libsql.ring file from the load directory
remove(exefolder() + "load" + cPathSep + "libsql.ring")

# Remove the libsql.ring file from the Ring2EXE libs directory
remove(exefolder() + ".." + cPathSep + "tools" + cPathSep + "ring2exe" + cPathSep + "libs" + cPathSep + "libsql.ring")

# Change current directory to the samples directory
chdir(exefolder() + ".." + cPathSep + "samples")

# Remove the UsingLibSQL directory if it exists
if direxists("UsingLibSQL")
	OSDeleteFolder("UsingLibSQL")
ok