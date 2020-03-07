rd /s /q .PACKAGE
rd /s /q .TEMP
mkdir .TEMP
copy content\*.* .TEMP
copy Release\winverge.exe .TEMP
cd .TEMP
..\zip.exe -X -r winverge.zip *.*
cd ..
mkdir .PACKAGE
move .TEMP\winverge.zip .PACKAGE
rd /s /q .TEMP