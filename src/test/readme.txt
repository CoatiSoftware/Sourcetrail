---------
Debugging
---------
To debug unit tests remove the post-build event, set the execution directory for 'Coati_test' to '.../Coati/bin/test' and run 'Coati_test' as startup project


Post build event (if you manage to delete it without saving it somewhere):

setlocal
cd $(ProjectDir)../../bin/test/
$(OutDir)$(TargetName)$(TargetExt)
if %errorlevel% neq 0 goto :cmEnd
:cmEnd
endlocal & call :cmErrorLevel %errorlevel% & goto :cmDone
:cmErrorLevel
exit /b %1
:cmDone
if %errorlevel% neq 0 goto :VCEnd