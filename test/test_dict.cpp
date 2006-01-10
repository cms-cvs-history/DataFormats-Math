#include <Reflex/Type.h>
#include <Cintex/Cintex.h>
#include <iostream>
#include <string>
using namespace seal::reflex;
using namespace std;

void dumpClass( const string & className ) {
  Type t = Type::byName( className );
  if ( t ) {
    if ( t.isClass() ) cout << "Found class: " << className << endl;
    for ( member_iterator mi = t.member_begin(); mi != t.member_end(); ++mi ) {
      switch ( (*mi).memberType() ) {
      case DATAMEMBER :
	cout << "Attribute: " << (*mi).name() << " at offset " 
	     << (*mi).offset() << endl;
	break;
      case FUNCTIONMEMBER : 
	cout << "Method: " << (*mi).name() << " has " 
	     << (*mi).parameterCount() << " parameters " << endl;
	break;
      default :
	cout << "This should never happen" << endl;
      }
    }  
  } else {
    cout << "Class: " << className << " not found" << endl;
  }
}

int main() {
  seal::cintex::Cintex::enable();
  dumpClass( "ROOT::Math::PositionVector3D<ROOT::Math::Cartesian3D<double> >" );
  dumpClass( "math::Vector<1>" );
  dumpClass( "math::Vector<2>" );
  dumpClass( "math::Vector<3>" );

  return 0;
}
