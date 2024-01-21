@echo off
rmdir Intermediate /s /q
rmdir DerivedDataCache /s /q
rmdir Binaries /s /q
rmdir .vs /s /q
rmdir .idea /s /q

del /q *.sln