echo     Internal documentation:
if not exist ..\doc\internal mkdir ..\doc\internal
perl C:\Dev\NaturalDocs-1.51\NaturalDocs -i . -o HTML ../doc/internal -p InternalDocs

echo     Public documentation:
if not exist ..\doc\public mkdir ..\doc\public
perl C:\Dev\NaturalDocs-1.51\NaturalDocs -i . -o HTML ../doc/public -p PublicDocs
