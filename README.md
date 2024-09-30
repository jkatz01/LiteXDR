# LiteXDR

how to run:
### option 1: 

in VM: run `bcdedit /set testsigning on` and restart to enable Test Mode on windows

in VM: use `devcon install KMDFXDRDriver.inf Root/KMDFXDRDriver`

### option 2:
in cmd:

sc delete xdrdriver

sc create xdrdriver type=kernel binpath="D:\Documents\LiteXDR\KMDF XDR Driver\x64\Release\KMDFXDRDriver.sys"

bcdedit /set testsigning on (restart pc after this)

sc start xdrdriver