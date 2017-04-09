set PATH=C:\Program Files (x86)\Microsoft Visual Studio 14.0\VC\bin;C:\Program Files (x86)\Microsoft Visual Studio 14.0\Common7\IDE;C:\Program Files (x86)\Windows Kits\8.1\bin\x86;C:\D\dmd2\windows\bin;%PATH%
dmd -g -debug -X -Xf"Win32\Debug\web_server_front.json" -deps="Win32\Debug\web_server_front.dep" -c -of"Win32\Debug\web_server_front.obj" main.d
if errorlevel 1 goto reportError

set LIB="C:\D\dmd2\windows\bin\..\lib"
echo. > c:\users\nicholas\DOCUME~1\VISUAL~1\Projects\WEB_SE~1\WEB_SE~1\Win32\Debug\WEB_SE~1.LNK
echo "Win32\Debug\web_server_front.obj","Win32\Debug\web_server_front.exe","Win32\Debug\web_server_front.map",user32.lib+ >> c:\users\nicholas\DOCUME~1\VISUAL~1\Projects\WEB_SE~1\WEB_SE~1\Win32\Debug\WEB_SE~1.LNK
echo kernel32.lib/NOMAP/CO/NOI/DELEXE >> c:\users\nicholas\DOCUME~1\VISUAL~1\Projects\WEB_SE~1\WEB_SE~1\Win32\Debug\WEB_SE~1.LNK

"C:\Program Files (x86)\VisualD\pipedmd.exe" -deps Win32\Debug\web_server_front.lnkdep C:\D\dmd2\windows\bin\link.exe @c:\users\nicholas\DOCUME~1\VISUAL~1\Projects\WEB_SE~1\WEB_SE~1\Win32\Debug\WEB_SE~1.LNK
if errorlevel 1 goto reportError
if not exist "Win32\Debug\web_server_front.exe" (echo "Win32\Debug\web_server_front.exe" not created! && goto reportError)

goto noError

:reportError
echo Building Win32\Debug\web_server_front.exe failed!

:noError
