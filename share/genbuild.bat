@echo off

git log -n 1 --pretty=format:"%%ai" > git_log_datetime.txt
set /p datetime= < git_log_datetime.txt 
del git_log_datetime.txt

echo #define BUILD_DATE "%datetime%" > %1