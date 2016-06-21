
:: Header files
for /r %%f in (*.h) do (
    clang-format %%f > %%f
     @echo %%f
)
:: tcc files

:: Source files
