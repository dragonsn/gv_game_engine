echo "%~dp0"
cd "%~dp0"
for %%x in (%*) do (
   echo %%x
   gv_base_unit_test.exe tool_c_to_cpp console no_stop  -%%x tool 

)

pause
