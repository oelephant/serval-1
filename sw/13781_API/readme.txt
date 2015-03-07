
13781 API  Release Notes                     January 19, 2015
------------------------

This API release represents a snapshot of the 13781 API. Feedback and 
comments on the API are welcomed.

The intent of the 13781  API is to provide a thin layer (wrapper) over the
actual 13781 hardware; and to provide a basic set of functions to make
programming and using the 13781 significantly easier by abstracting complex 
tasks into pre-written functions. The 13781 API does not perform any extensive 
error checking. Some minor error checking is done, and in many cases asserts 
(seASSERT) are used to catch and trap major coding errors.

At a high level the API package consists of the following components:
- The core API routines and low-level 13781 accessor functions
  - Settings to select the interface type
  - An OS-specific support code
  - Epson Evaluation Card support code
  - An auxiliary library of useful functions and tasks
- Test and sample code demonstrating the use of the 13771 API
- Project files to build and develop the API in MS Visual Studio 2005 environment

The structure of the 13781 API folders is as follows:
- api            Contains the primary 13771 API source code
- documentation  Contains 13781 API documentation/notes files
- hcl            Contains the Hardware Control Layer
                 (register and memory reads and writes)
- include        Contains the 13781 API header files (interface)
- os             Contains the OS-specific coding layer
- pcibridge      Contains the Epson PCI Eval Card layer (used by OS)
- samples        Contains test/sample code demonstrating the API
  - testdislay
  - testhcl
  - testinit
- usbbridge      Contains the Epson USB Eval Card layer (used by OS)
- seaux          Contains optional auxiliary routines
- usbbridge      Contains the USB hardware interface layer

Compile-time options are controlled in the settings.h file, located in the 
include folder. Any changes to the settings file will require the software 
to be re-compiled.

It is important to note that the API needs to be configured with CFG (a
configuration utility) to setup a default 13781 state. To configure the API,
replace initdata.c with a newly exported one from CFG. The CFG utitily is
downloaded separately.


NOTES
-----

1. The 13781 API development is static however additional functions may be
   added as needed when new requirements are identified. Any errors and/or 
   omissions to the basic API functionality will be corrected and updated 
   in future releases of the API.
   
   Notably outstanding from this release is support for any interface other
   than the USB evaluation board. API code is added to the package as it is
   developed and tested. To date we do not have a PCI inteface eval board
   for development and testing.

2. As additional high-level features are identified, these may be coded into
   the API and/or the auxiliary library in future 13781 API releases.

3. The 13781 API is designed to be a starting point template for system
   integrators. That is, it is expected that developers will modify the API
   to suit their individual needs and requirements.

4. The 13781 API was designed and coded to function on platforms that natively 
   support 32-bits. If the API is to be used on a 16-bit platform, it is the 
   system developers responsibility to modify the code to support their platform.

5. Not every single field and/or bit in the 13781 register set will have an
   associated API function. The API attempts to support the most common and
   most needed functions of the 13781. This is by design.  Applications and
   operating environments can always access 13781 registers directly by using
   the low-level register access functions to fine tune operation.

6. It is expected that developers will need re-write the OS layer, as the API
   only demonstrates the 13781 functionality on WIN32 platforms.

7. It is belived the API documentation is complete. Any errors or ommissions will
   be corrected in a future release.

8. The 13781 API was designed and coded to compile with a good-quality ANSI 
   compiler. Older, less robust compilers many generate errors or warnings
   with the API code. It is the developers responsibility to modify the API 
   code to comply with their development environment.
   
   
  RELEASE HISTORY

  2015-Jan-19
  - Removed camera code, which isn't supported by 781.

  2010-June-29
  - Repackaged with the updated version of the 13781CFG program.

  2010-June-08
  - UPdated the USB initialization code to match the actual configuration
    on customer evaluation boards.
  
  2009-June-25
  - Beta release
