'''
Extracts XML schema from a .cpp file and saves it in an XML file.
Command line arguments:
argv[1]  input .cpp file
argv[2]  output XML file
'''

import re
import sys

with (open(sys.argv[1], 'r') as cpp, 
      open(sys.argv[2], 'w') as xsd):
    schema = cpp.readlines()
    # index of schema's first line
    start = next((i for i in range(len(schema)) if
                  re.match(r'<\?xml ', schema[i]) is not None), 0)
    # index of schema's last line
    end = next((i for i in reversed(range(len(schema))) if
                  re.match(r'</xs:schema>', schema[i]) is not None), 0)
    # comment goes before the XML declaration so technically the resulting
    #   schema is not a valid XML
    xsd.write('''<!-- This is a copy of the schema used internally by the "songbook" program. 
It was generated during program build and changes to it will have no effect 
on XML validation performed by the program. -->
''')
    xsd.writelines(schema[start:(end + 1)])
