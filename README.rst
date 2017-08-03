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
    strVariable = "Mabuhay " + 'Ka' #concatenation

    #function definition
    fd hello_world(var)
        intVariable = 0
        #if statement
        if(var = 'Mabuhay Ka')

            #loop statement
            fl (1 to 7)
                intVariable = intVariable + 1
                sc(intVariable) #change text color
                p(var) #It will print "Mabuhay Ka" 5 times
                zzz(2000) #sleep for 2 seconds
            lf
            
            sc(CNRM) #reset the text color
        e
            p('Not Mabuhay Ka')
        fi
    df

    hello_world(strVariable) #call function
    
Language Reference
==================

Click here_

.. _here: https://github.com/six519/titik/blob/master/language_reference.rst