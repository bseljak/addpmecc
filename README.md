# addpmecc
Utils to add PMECC block to nand flash image for Atmel SAM
Usage:
addpmecc <eccOffset:36> <sectorSize:512> <eccBitReq:4> <spareSize:64> <nbSectorPerPage:4> <infilename> <outfilename>
or 
For default parameters: 
addpmecc <infilename> <outfilename>
