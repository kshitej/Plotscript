#include "atom.hpp"

#include <sstream>
#include <cctype>
#include <cmath>
#include <limits>

Atom::Atom(): m_type(NoneKind) {}

Atom::Atom(double value){

  setNumber(value);
}

Atom::Atom(std::complex<double> value): Atom(){
  setComplex(value);
}

Atom::Atom(const Token & token): Atom(){
  
  // is token a number?
  double temp;
  std::istringstream iss(token.asString());
  if(iss >> temp){
    // check for trailing characters if >> succeeds
    if(iss.rdbuf()->in_avail() == 0){
      setNumber(temp);
    }
  }
  else{ // else assume symbol
    // make sure does not start with number
    if(!std::isdigit(token.asString()[0])){
			std::string temp = token.asString();
			
			setSymbol(temp);
    }
  }
}

Atom::Atom(const std::string & value): Atom() {

	setSymbol(value);
}

Atom::Atom(const Atom & x): Atom(){
  if(x.isNumber()){
    setNumber(x.numberValue);
  }
  else if(x.isSymbol()){
    setSymbol(x.stringValue);
  }
  else if(x.isComplex()){
    setComplex(x.complexValue);
  }
}

Atom & Atom::operator=(const Atom & x){

  if(this != &x){
    if(x.m_type == NoneKind){
      m_type = NoneKind;
    }
    else if(x.m_type == NumberKind){
      setNumber(x.numberValue);
    }
    else if(x.m_type == SymbolKind){
      setSymbol(x.stringValue);
    }
    else if(x.m_type == ComplexKind){
      setComplex(x.complexValue);
    }
  }
  return *this;
}
  
Atom::~Atom(){

  // we need to ensure the destructor of the symbol string is called
  if(m_type == SymbolKind){
    stringValue.~basic_string();
  }
}

bool Atom::isNone() const noexcept{
  return m_type == NoneKind;
}

bool Atom::isComplex() const noexcept{
  return m_type == ComplexKind;
}

bool Atom::isNumber() const noexcept{
  return m_type == NumberKind;
}

bool Atom::isSymbol() const noexcept{
  return m_type == SymbolKind;
}  


void Atom::setNumber(double value){

  m_type = NumberKind;
  numberValue = value;
}

void Atom::setComplex(std::complex<double> value){
  m_type = ComplexKind;
  complexValue = value;
}

void Atom::setSymbol(const std::string & value){

  // we need to ensure the destructor of the symbol string is called
  if(m_type == SymbolKind){
    stringValue.~basic_string();
  }
	
  m_type = SymbolKind;

  // copy construct in place
  new (&stringValue) std::string(value);
}


double Atom::asNumber() const noexcept{

  return (m_type == NumberKind) ? numberValue : 0.0;  
}

std::complex<double> Atom::asComplex() const noexcept{
  std::complex<double> temp = 0;
  return (m_type == ComplexKind) ? complexValue : temp;
}

std::string Atom::asSymbol() const noexcept{

  std::string result;

  if(m_type == SymbolKind){
    result = stringValue;
  }

  return result;
}


bool Atom::operator==(const Atom & right) const noexcept{
  
  if(m_type != right.m_type) return false;

  switch(m_type){
  case NoneKind:
    if(right.m_type != NoneKind) return false;
    break;
  case NumberKind:
    {
      if(right.m_type != NumberKind) return false;
      double dleft = numberValue;
      double dright = right.numberValue;
      double diff = fabs(dleft - dright);
      if(std::isnan(diff) ||
	    (diff > std::numeric_limits<double>::epsilon())) 
        return false;
    }
    break;
    
  case SymbolKind:
    {
      if(right.m_type != SymbolKind) return false;

      return stringValue == right.stringValue;
    }
    break;



  case ComplexKind:
    {
      if(right.m_type != ComplexKind) return false;
      std::complex<double> dleft = complexValue;
      std::complex<double> dright = right.complexValue;
      double diff  = abs(dleft - dright);
      if(std::isnan(diff) ||
	    (diff > std::numeric_limits<double>::epsilon())) 
        return false;
    }
    break;

  default:
    return false;
  }

  return true;
}

bool operator!=(const Atom & left, const Atom & right) noexcept{
  
  return !(left == right);
}


std::ostream & operator<<(std::ostream & out, const Atom & a){

  if(a.isNumber()){
    out << a.asNumber();
  }
  if(a.isSymbol()){
    out << a.asSymbol();
  }
  if(a.isComplex())
  {
    std::complex<double>temp = a.asComplex();
    out << temp;
  }
  return out;
}
