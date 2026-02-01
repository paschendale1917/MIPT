set "current_path=%~dp0"
powershell -Command "Start-Process cmd.exe -Verb RunAs -ArgumentList '/k cd /d \"%current_path%\"'"