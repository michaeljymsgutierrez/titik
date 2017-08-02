titik
=====

Titik Programming Language/Interpreter

Intro
=====

This is for experimental and educational purpose only. ;)

Linux/OSX/FreeBSD Requirement
=============================

GNU Compiler Collection (GCC)

Building On Linux/OSX/FreeBSD Machine
=====================================

::

    make compile

Windows Requirement
===================

Visual Studio

Building On Windows Machine
===========================

Open `Developer Command Prompt for Visual Studio` and run `build_win.cmd`

Sample Code
===========
::

    \
        Multiline comment
        (Hello World Code)
        Source code at: https://github.com/six519/titik
    \

    floatVariable = 25.55
    strVariable = "Hello " + 'World' #concatenation

    #function definition
    fd hello_world(var)
        intVariable = 0
        #if statement
        if(var = 'Hello World')

            #loop statement
            fl (1 to 7)
                intVariable = intVariable + 1
                sc(intVariable) #change text color
                p(var) #It will print "Hello World" 5 times
                zzz(2000) #sleep for 2 seconds
            lf
            
            sc(CNRM) #reset the text color
        e
            p('Not Hello World')
        fi
    df

    hello_world(strVariable) #call function