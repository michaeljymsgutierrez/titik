Hello World Code
================

.. image:: http://ferdinandsilva.com/media/mabuhay.png

Comments
========

**Single Line Comment**

.. image:: http://ferdinandsilva.com/media/comment1.png

**Multiline Comment**

.. image:: http://ferdinandsilva.com/media/comment2.png

Variables
=========

**Rules:**

- A variable name must start with a letter or the underscore character
- A variable name cannot start with a number
- A variable name can only contain alpha-numeric characters and underscores (A-z, 0-9, and _ )
- Variable names are case-sensitive (name and nAme are two different variables)

**Data Types:**

- *Integer* - a non-decimal number between −2,147,483,647 and 2,147,483,647

.. image:: http://ferdinandsilva.com/media/integer.png

- *String* - a sequence of characters. A string can be any text inside quotes. You can use single or double quotes.

.. image:: http://ferdinandsilva.com/media/string.png

- *Float* - a number with a decimal point.

.. image:: http://ferdinandsilva.com/media/float.png

- *Nil* - a special data type that represents a variable with no value

.. image:: http://ferdinandsilva.com/media/nil.png

**Constant:**

It has a name starting with an uppercase character.

.. image:: http://ferdinandsilva.com/media/constant.png

Operators
=========

**Arithmetic Operators**

+----------+-----------------+-----------------------------------------------------------------------+
| Operator |       Name      |                  Example                                              |
+==========+=================+=======================================================================+
|      \+  |    Addition     | .. image:: http://ferdinandsilva.com/media/sum.png                    |
+----------+-----------------+-----------------------------------------------------------------------+
|      \-  |   Subtraction   | .. image:: http://ferdinandsilva.com/media/difference.png             |
+----------+-----------------+-----------------------------------------------------------------------+
|      \*  | Multiplication  | .. image:: http://ferdinandsilva.com/media/product.png                |
+----------+-----------------+-----------------------------------------------------------------------+
|       /  |   Division      | .. image:: http://ferdinandsilva.com/media/quotient.png               |
+----------+-----------------+-----------------------------------------------------------------------+

**Comparison Operators**

+----------+-----------------+-----------------------------------------------------------------------+
| Operator |       Name      |                  Example                                              |
+==========+=================+=======================================================================+
|    \=    | Equal           | .. image:: http://ferdinandsilva.com/media/equal.png                  |
+----------+-----------------+-----------------------------------------------------------------------+
|     >    | Greater than    | .. image:: http://ferdinandsilva.com/media/greater.png                |
+----------+-----------------+-----------------------------------------------------------------------+
|     <    | Less than       | .. image:: http://ferdinandsilva.com/media/less.png                   |
+----------+-----------------+-----------------------------------------------------------------------+

If Statement
============

**If Statement**

Executes some code if one condition is true.

**Syntax**
::
    if (condition)
        code to be executed if condition is true
    fi

**Example**

.. image:: http://ferdinandsilva.com/media/equal.png

**If...Else Statement**

Executes some code if condition is true and another code if that condition is false.

**Syntax**
::
    if (condition)
        code to be executed if condition is true
    e 
        code to be executed if condition is false
    fi

**Example**

.. image:: http://ferdinandsilva.com/media/ifelse.png

**If...ElseIf...Else Statement**

Executes different codes for more than two conditions.

**Syntax**
::
    if (condition)
        code to be executed if this condition is true
    ef (condition)
        code to be executed if this condition is true
    e
        code to be executed if all conditions are false
    fi

**Example**

.. image:: http://ferdinandsilva.com/media/ifelseif.png

Looping Statements
==================

**Forward For Loop**

Execute a block of code a specified number of times where start counter is lower than end counter.

**Syntax**
::
    fl (start counter to end counter)
        code to be executed
    lf

**Example**

.. image:: http://ferdinandsilva.com/media/forward.png

**Backward For Loop**

Execute a block of code a specified number of times where start counter is greater than end counter.

**Syntax**
::
    fl (start counter to end counter)
        code to be executed
    lf

**Example**

.. image:: http://ferdinandsilva.com/media/backward.png

**While Loop**

Execute a block of code while the specified condition is true.

**Syntax**
::
    wl (condition)
        code to be executed
    lw

**Example**

.. image:: http://ferdinandsilva.com/media/while.png

**Break Statement**

When a break statement is encountered inside a loop, the loop is immediately terminated and the program control resumes at the next statement following the loop.

**Example**

.. image:: http://ferdinandsilva.com/media/break.png

Functions
=========

**Syntax**
::
    fd functionName(parameter1, parameter2)
        code to be executed
    df

**Example 1 (Function without parameter & return)**

.. image:: http://ferdinandsilva.com/media/function1.png

**Example 2 (Function with parameters & return)**

.. image:: http://ferdinandsilva.com/media/function2.png