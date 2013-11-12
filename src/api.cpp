// -*- mode: C++; c-indent-level: 4; c-basic-offset: 4; indent-tabs-mode: nil; -*-
// jedit: :folding=explicit:
//
// api.cpp: Rcpp R/C++ interface class library -- Rcpp api
//
// Copyright (C) 2012 - 2013  Dirk Eddelbuettel and Romain Francois
//
// This file is part of Rcpp.
//
// Rcpp is free software: you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 2 of the License, or
// (at your option) any later version.
//
// Rcpp is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Rcpp.  If not, see <http://www.gnu.org/licenses/>.
  
#include <Rcpp.h>

#include "internal.h" 

// for R_ObjectTable
#include <R_ext/Callbacks.h>
#include <R_ext/PrtUtil.h>

#ifdef RCPP_HAS_DEMANGLING
#include <cxxabi.h>
#endif

// {{{ Rcpp api classes
namespace Rcpp {
         
    // [[Rcpp::register]]
    SEXP Rcpp_PreserveObject(SEXP x){ 
        if( x != R_NilValue ) {
            R_PreserveObject(x); 
        }
        return x ;
    }
    
    // [[Rcpp::register]]
    void Rcpp_ReleaseObject(SEXP x){
        if (x != R_NilValue) {
            R_ReleaseObject(x); 
        }
    }
    
    // [[Rcpp::register]]
    SEXP Rcpp_ReplaceObject(SEXP x, SEXP y){
        if( x == R_NilValue ){
            Rcpp_PreserveObject( y ) ;    
        } else if( y == R_NilValue ){
            Rcpp_ReleaseObject( x ) ;
        } else {
            // if we are setting to the same SEXP as we already have, do nothing 
            if (x != y) {
                
                // the previous SEXP was not NULL, so release it 
                Rcpp_ReleaseObject(x);
                
                // the new SEXP is not NULL, so preserve it 
                Rcpp_PreserveObject(y);
                        
            }
        }
        return y ;
    }   
    
    // [[Rcpp::register]]  
    std::string demangle( const std::string& name ){
        #ifdef RCPP_HAS_DEMANGLING
            std::string real_class ;
            int status =-1 ;
            char *dem = 0;
            dem = abi::__cxa_demangle(name.c_str(), 0, 0, &status);
            if( status == 0 ){
                real_class = dem ;
                free(dem);
            } else {
                real_class = name ;
            }
            return real_class ;
        #else
            return name ;
        #endif    
    }
      
    // [[Rcpp::register]]
    const char * type2name(SEXP x) {
        switch (TYPEOF(x)) {
        case NILSXP:	return "NILSXP";
        case SYMSXP:	return "SYMSXP";
        case RAWSXP:	return "RAWSXP";
        case LISTSXP:	return "LISTSXP";
        case CLOSXP:	return "CLOSXP";
        case ENVSXP:	return "ENVSXP";
        case PROMSXP:	return "PROMSXP";
        case LANGSXP:	return "LANGSXP";
        case SPECIALSXP:	return "SPECIALSXP";
        case BUILTINSXP:	return "BUILTINSXP";
        case CHARSXP:	return "CHARSXP";
        case LGLSXP:	return "LGLSXP";
        case INTSXP:	return "INTSXP";
        case REALSXP:	return "REALSXP";
        case CPLXSXP:	return "CPLXSXP";
        case STRSXP:	return "STRSXP";
        case DOTSXP:	return "DOTSXP";
        case ANYSXP:	return "ANYSXP";
        case VECSXP:	return "VECSXP";
        case EXPRSXP:	return "EXPRSXP";
        case BCODESXP:	return "BCODESXP";
        case EXTPTRSXP:	return "EXTPTRSXP";
        case WEAKREFSXP:	return "WEAKREFSXP";
        case S4SXP:		return "S4SXP";
        default:
        return "<unknown>";
        }
    }
    
    // [[Rcpp::internal]]
    SEXP rcpp_capabilities(){
        Shield<SEXP> cap   = Rf_allocVector( LGLSXP, 9 ) ;
        Shield<SEXP> names = Rf_allocVector( STRSXP, 9 ) ;
        #ifdef HAS_VARIADIC_TEMPLATES
            LOGICAL(cap)[0] = TRUE ;
        #else
            LOGICAL(cap)[0] = FALSE ;
        #endif
        #ifdef HAS_CXX0X_INITIALIZER_LIST
            LOGICAL(cap)[1] = TRUE ;
        #else
            LOGICAL(cap)[1] = FALSE ;
        #endif
        /* exceptions are always supported */
        LOGICAL(cap)[2] = TRUE ;
        
        #ifdef HAS_TR1_UNORDERED_MAP
            LOGICAL(cap)[3] = TRUE ;
        #else
           LOGICAL(cap)[3] = FALSE ;
        #endif
        
        #ifdef HAS_TR1_UNORDERED_SET
            LOGICAL(cap)[4] = TRUE ;
        #else
            LOGICAL(cap)[4] = FALSE ;
        #endif
        
        LOGICAL(cap)[5] = TRUE ;
        
        #ifdef RCPP_HAS_DEMANGLING
            LOGICAL(cap)[6] = TRUE ;
        #else
           LOGICAL(cap)[6] = FALSE ;
        #endif
        
           LOGICAL(cap)[7] = FALSE ;
        
        #ifdef RCPP_HAS_LONG_LONG_TYPES
            LOGICAL(cap)[8] = TRUE ;
        #else
            LOGICAL(cap)[8] = FALSE ;
        #endif
        
        SET_STRING_ELT(names, 0, Rf_mkChar("variadic templates") ) ;
        SET_STRING_ELT(names, 1, Rf_mkChar("initializer lists") ) ;
        SET_STRING_ELT(names, 2, Rf_mkChar("exception handling") ) ;
        SET_STRING_ELT(names, 3, Rf_mkChar("tr1 unordered maps") ) ;
        SET_STRING_ELT(names, 4, Rf_mkChar("tr1 unordered sets") ) ;
        SET_STRING_ELT(names, 5, Rf_mkChar("Rcpp modules") ) ;
        SET_STRING_ELT(names, 6, Rf_mkChar("demangling") ) ;
        SET_STRING_ELT(names, 7, Rf_mkChar("classic api") ) ;
        SET_STRING_ELT(names, 8, Rf_mkChar("long long") ) ;
        Rf_setAttrib( cap, R_NamesSymbol, names ) ;
        return cap ;
    }
 

    // [[Rcpp::internal]]
    SEXP rcpp_can_use_cxx0x(){ 
        #ifdef HAS_VARIADIC_TEMPLATES
            return Rf_ScalarLogical( TRUE );
        #else
            return Rf_ScalarLogical( FALSE );
        #endif
    }
    
    
    
    
    // {{{ DataFrame
    namespace internal{
        inline SEXP empty_data_frame(){
            SEXP dataFrameSym = ::Rf_install( "data.frame"); // cannot be gc()ed once in symbol table
            return ::Rf_eval( ::Rf_lang1( dataFrameSym ), R_GlobalEnv ) ;       
        }
    }
    
    DataFrame::DataFrame(): List( internal::empty_data_frame() ){}
    DataFrame::DataFrame(SEXP x) : List(x){
        set_sexp(x) ;
    }  
    DataFrame::DataFrame( const DataFrame& other): List(other.asSexp()) {}
    DataFrame::DataFrame( const RObject::SlotProxy& proxy ) { set_sexp(proxy); }
    DataFrame::DataFrame( const RObject::AttributeProxy& proxy ) { set_sexp(proxy); }
              
    DataFrame& DataFrame::operator=( DataFrame& other) {
        set_sexp( other.asSexp() ) ;
        return *this ;
    }
            
    DataFrame& DataFrame::operator=( SEXP x) {
        set_sexp(x) ;
        return *this ;
    }
    DataFrame::~DataFrame(){}     
    void DataFrame::set_sexp(SEXP x) {
        if( ::Rf_inherits( x, "data.frame" )){
            setSEXP( x ) ;
        } else{
            SEXP y = internal::convert_using_rfunction( x, "as.data.frame" ) ;
            setSEXP( y ) ;
        }
        List::update_vector() ;
    } 
    int DataFrame::nrows() const { return Rf_length( VECTOR_ELT(m_sexp, 0) ); }
        
    DataFrame DataFrame::from_list( Rcpp::List obj ){
        bool use_default_strings_as_factors = true ;
        bool strings_as_factors = true ;
        int strings_as_factors_index = -1 ;
        int n = obj.size() ;
        CharacterVector names = obj.attr( "names" ) ;
        if( !names.isNULL() ){
            for( int i=0; i<n; i++){
                if( names[i] == "stringsAsFactors" ){
                    strings_as_factors_index = i ;
                    use_default_strings_as_factors = false ;        
                    if( !as<bool>(obj[i]) ) strings_as_factors = false ;
                    break ;         
                }
            }
        }
        if( use_default_strings_as_factors ) 
            return DataFrame(obj) ;
        SEXP as_df_symb = Rf_install("as.data.frame");
        SEXP strings_as_factors_symb = Rf_install("stringsAsFactors");
        
        obj.erase(strings_as_factors_index) ;
        names.erase(strings_as_factors_index) ;
        obj.attr( "names") = names ;
        SEXP call  = PROTECT( Rf_lang3(as_df_symb, obj, wrap( strings_as_factors ) ) ) ;
        SET_TAG( CDDR(call),  strings_as_factors_symb ) ;   
        SEXP res = PROTECT( Rcpp_eval( call ) ) ; 
        DataFrame out( res ) ;
        UNPROTECT(2) ;
        return out ;
    }
    
    // }}}
    
} // namespace Rcpp
// }}}


// {{{ utilities (from RcppCommon.cpp)

namespace Rcpp{
namespace internal{

	SEXP convert_using_rfunction(SEXP x, const char* const fun) {
        SEXP res = R_NilValue ;
        try{
            SEXP funSym = Rf_install(fun);
            res = Rcpp_eval( Rf_lang2( funSym, x ) ) ;
        } catch( eval_error& e){
            throw ::Rcpp::not_compatible( std::string("could not convert using R function : ") + fun  ) ;
        }
        return res;
    }
    
    SEXP eval_methods<EXPRSXP>::eval(){
        SEXP xp = ( static_cast<ExpressionVector&>(*this) ).asSexp() ;
        SEXP evalSym = Rf_install( "eval" );
        return Rcpp_eval( Rf_lang2( evalSym, xp ) ) ;
    }
    
    SEXP eval_methods<EXPRSXP>::eval( SEXP env ){
        SEXP xp = ( static_cast<ExpressionVector&>(*this) ).asSexp() ;
        SEXP evalSym = Rf_install( "eval" );
        return Rcpp_eval( Rf_lang3( evalSym, xp, env ) ) ;
    }
	
} // internal
} // Rcpp

SEXP as_character_externalptr(SEXP xp){
	char buffer[20] ;
	sprintf( buffer, "%p", (void*)EXTPTR_PTR(xp) ) ;
	return Rcpp::wrap( (const char*)buffer ) ;
}

SEXP exception_to_try_error( const std::exception& ex ){
    return string_to_try_error(ex.what());
}

SEXP string_to_try_error( const std::string& str){

    using namespace Rcpp;
	
    // form simple error condition based on a string
    SEXP simpleErrorExpr = PROTECT(::Rf_lang2(::Rf_install("simpleError"), Rf_mkString(str.c_str())));
    SEXP simpleError = PROTECT(Rf_eval(simpleErrorExpr, R_GlobalEnv));
	
    SEXP tryError = PROTECT( Rf_mkString( str.c_str() ) ) ;
    Rf_setAttrib( tryError, R_ClassSymbol, Rf_mkString("try-error") ) ; 
    Rf_setAttrib( tryError, Rf_install( "condition") , simpleError ) ; 
    
    // unprotect and return
    UNPROTECT(3);
    return tryError;
}

const char* short_file_name(const char* file){
    std::string f(file) ;
    size_t index = f.find("/include/") ;
    if( index != std::string::npos ){ f = f.substr( index + 9 ) ;}
    return f.c_str() ;
}

#if defined(__GNUC__)
#if defined(WIN32) || defined(__FreeBSD__) || defined(__NetBSD__) || defined(__OpenBSD__) || defined(__CYGWIN__) || defined(__sun)
// Simpler version for Windows and *BSD 
SEXP stack_trace( const char* file, int line ){
    Rcpp::List trace = Rcpp::List::create( 
    	Rcpp::Named( "file"  ) = file, 
    	Rcpp::Named( "line"  ) = line, 
    	Rcpp::Named( "stack" ) = "C++ stack not available on this system" ) ;
    trace.attr("class") = "Rcpp_stack_trace" ;
    return trace ;
}
#else // ! (defined(WIN32) || defined(__FreeBSD__) || defined(__NetBSD__) || defined(__OpenBSD__) || defined(__CYGWIN__) || defined(__sun)
#include <execinfo.h>
#include <cxxabi.h>

static std::string demangler_one( const char* input){
    static std::string buffer ;
    buffer = input ;
    buffer.resize( buffer.find_last_of( '+' ) - 1 ) ;
    buffer.erase(
        buffer.begin(), 
        buffer.begin() + buffer.find_last_of( ' ' ) + 1
    ) ;
    return demangle( buffer) ;
}

/* inspired from http://tombarta.wordpress.com/2008/08/01/c-stack-traces-with-gcc/  */ 
SEXP stack_trace( const char *file, int line) {
    const size_t max_depth = 100;
    size_t stack_depth;
    void *stack_addrs[max_depth];
    char **stack_strings;

    stack_depth = backtrace(stack_addrs, max_depth);
    stack_strings = backtrace_symbols(stack_addrs, stack_depth);

    std::string current_line ;
    
    Rcpp::CharacterVector res( stack_depth - 1) ;
    std::transform( 
    	stack_strings + 1, stack_strings + stack_depth, 
    	res.begin(), 
    	demangler_one 
    	) ;
    free(stack_strings); // malloc()ed by backtrace_symbols
    
    Rcpp::List trace = Rcpp::List::create( 
    	Rcpp::Named( "file"  ) = file, 
    	Rcpp::Named( "line"  ) = line, 
    	Rcpp::Named( "stack" ) = res ) ;
    trace.attr("class") = "Rcpp_stack_trace" ;
    return trace ;
}
#endif 
#else /* !defined( __GNUC__ ) */
SEXP stack_trace( const char *file, int line) {
	return R_NilValue ;
}
#endif   
// }}}

// {{{ coercion

namespace Rcpp{ 
namespace internal{

template <> int r_coerce<INTSXP,INTSXP>(int from) { return from ; }
template <> int r_coerce<LGLSXP,LGLSXP>(int from) { return from ; }
template <> double r_coerce<REALSXP,REALSXP>(double from) { return from ; }
template <> Rcomplex r_coerce<CPLXSXP,CPLXSXP>(Rcomplex from) { return from ; }
template <> Rbyte r_coerce<RAWSXP,RAWSXP>(Rbyte from) { return from ; }

// -> INTSXP
template <> int r_coerce<LGLSXP,INTSXP>(int from){
	return (from==NA_LOGICAL) ? NA_INTEGER : from ;
}
template <> int r_coerce<REALSXP,INTSXP>(double from){
	if (ISNAN(from)) return NA_INTEGER;
	else if (from > INT_MAX || from <= INT_MIN ) {
		return NA_INTEGER;
	}
	return static_cast<int>(from);

}
template <> int r_coerce<CPLXSXP,INTSXP>(Rcomplex from){
	return r_coerce<REALSXP,INTSXP>(from.r) ;
}
template <> int r_coerce<RAWSXP,INTSXP>(Rbyte from){
	return static_cast<int>(from);
}

// -> REALSXP
template <> double r_coerce<LGLSXP,REALSXP>(int from){
	return from == NA_LOGICAL ? NA_REAL : static_cast<double>(from) ;
}
template <> double r_coerce<INTSXP,REALSXP>(int from){
	return from == NA_INTEGER ? NA_REAL : static_cast<double>(from) ; 
}
template <> double r_coerce<CPLXSXP,REALSXP>(Rcomplex from){
	return from.r ;
}
template <> double r_coerce<RAWSXP,REALSXP>(Rbyte from){
	return static_cast<double>(from) ;
}

// -> LGLSXP
template <> int r_coerce<REALSXP,LGLSXP>(double from){
	return ( from == NA_REAL ) ? NA_LOGICAL : (from!=0.0);
}
template <> int r_coerce<INTSXP,LGLSXP>(int from){
	return ( from == NA_INTEGER ) ? NA_LOGICAL : (from!=0);
}
template <> int r_coerce<CPLXSXP,LGLSXP>(Rcomplex from){
	if( from.r == NA_REAL ) return NA_LOGICAL ;
	if( from.r == 0.0 || from.i == 0.0 ) return FALSE ;
	return TRUE ;
}
template <> int r_coerce<RAWSXP,LGLSXP>(Rbyte from){
	if( from != static_cast<Rbyte>(0) ) return TRUE ;
	return FALSE ;
}

// -> RAWSXP
template <> Rbyte r_coerce<REALSXP,RAWSXP>(double from){
	if( from == NA_REAL) return static_cast<Rbyte>(0) ; 
	return r_coerce<INTSXP,RAWSXP>(static_cast<int>(from)) ;
}
template <> Rbyte r_coerce<INTSXP,RAWSXP>(int from){
	return (from < 0 || from > 255) ? static_cast<Rbyte>(0) : static_cast<Rbyte>(from) ;
}
template <> Rbyte r_coerce<CPLXSXP,RAWSXP>(Rcomplex from){
	return r_coerce<REALSXP,RAWSXP>(from.r) ;
}
template <> Rbyte r_coerce<LGLSXP,RAWSXP>(int from){
	return static_cast<Rbyte>(from == TRUE) ;
}

// -> CPLXSXP
template <> Rcomplex r_coerce<REALSXP,CPLXSXP>(double from){
	Rcomplex c ;
	if( from == NA_REAL ){
		c.r = NA_REAL; 
		c.i = NA_REAL;
	} else{
		c.r = from ;
		c.i = 0.0 ;
	}
	return c ;
}
template <> Rcomplex r_coerce<INTSXP,CPLXSXP>(int from){
	Rcomplex c ;
	if( from == NA_INTEGER ){
		c.r = NA_REAL; 
		c.i = NA_REAL;
	} else{
		c.r = static_cast<double>(from) ;
		c.i = 0.0 ;
	}
	return c ;
}
template <> Rcomplex r_coerce<RAWSXP,CPLXSXP>(Rbyte from){
	Rcomplex c ;
	c.r = static_cast<double>(from);
	c.i = 0.0 ;
	return c ;
}
template <> Rcomplex r_coerce<LGLSXP,CPLXSXP>(int from){
	Rcomplex c ;
	if( from == TRUE ){
		c.r = 1.0 ; c.i = 0.0 ;
	} else if( from == FALSE ){
		c.r = c.i = 0.0 ;
	} else { /* NA */
		c.r = c.i = NA_REAL;
	}
	return c ;
}

inline int integer_width( int n ){
    return n < 0 ? ( (int) ( ::log10( -n+0.5) + 2 ) ) : ( (int) ( ::log10( n+0.5) + 1 ) ) ;    
}

#define NB 1000
template <> const char* coerce_to_string<INTSXP>(int from){
    static char buffer[NB] ;
    snprintf( buffer, NB, "%*d", integer_width(from), from );
    return buffer ;
}
template <> const char* coerce_to_string<LGLSXP>(int from){
    return from == 0 ? "FALSE" : "TRUE" ;    
}
template <> const char* coerce_to_string<RAWSXP>(Rbyte from){
    static char buff[3];
    ::sprintf(buff, "%02x", from);
    return buff ;    
}

char* get_string_buffer(){
    static char buffer[MAXELTSIZE];
    return buffer ;    
}

static const char* dropTrailing0(char *s, char cdec) {
    /* Note that  's'  is modified */
    char *p = s;
    for (p = s; *p; p++) {
        if(*p == cdec) {
            char *replace = p++;
            while ('0' <= *p  &&  *p <= '9')
            if(*(p++) != '0')
                replace = p;
            if(replace != p)
                while((*(replace++) = *(p++))) ;
            break;
        }
    }
    return s;
}

template <> const char* coerce_to_string<REALSXP>(double x){
    //int w,d,e ;
    // cf src/main/format.c in R's sources:
    //   The return values are
    //     w : the required field width
    //     d : use %w.df in fixed format, %#w.de in scientific format
    //     e : use scientific format if != 0, value is number of exp digits - 1
    //
    //   nsmall specifies the minimum number of decimal digits in fixed format:
    //   it is 0 except when called from do_format.
    //Rf_formatReal( &x, 1, &w, &d, &e, 0 ) ;
    // we are no longer allowed to use this:
    //     char* tmp = const_cast<char*>( Rf_EncodeReal(x, w, d, e, '.') );
    // so approximate it poorly as
    static char tmp[128];
    snprintf(tmp, 127, "%f", x); 
    return dropTrailing0(tmp, '.');
}

template <> const char* coerce_to_string<CPLXSXP>(Rcomplex x){
    //int wr, dr, er, wi, di, ei;
    //Rf_formatComplex(&x, 1, &wr, &dr, &er, &wi, &di, &ei, 0);
    // we are no longer allowed to use this:
    //     Rf_EncodeComplex(x, wr, dr, er, wi, di, ei, '.' );
    // so approximate it poorly as
    static char tmp1[128], tmp2[128], tmp3[256];
    //snprintf(tmp, 127, "%*.*f+%*.*fi", wr, dr, x.r, wi, di, x.i);
    //snprintf(tmp, 127, "%f+%fi", x.r, x.i); // FIXEM: barebones default formatting
    snprintf(tmp1, 127, "%f", x.r); 
    snprintf(tmp2, 127, "%f", x.i); 
    snprintf(tmp3, 255, "%s+%si", dropTrailing0(tmp1, '.'), dropTrailing0(tmp2, '.'));
    return tmp3;
}

} // internal
} // Rcpp

// }}}

// {{{ r_cast support
namespace Rcpp{
    namespace internal{
        
        template<> SEXP r_true_cast<INTSXP>(SEXP x) {
            switch( TYPEOF(x) ){
            case REALSXP:
            case RAWSXP:
            case LGLSXP:
            case CPLXSXP:
                return Rf_coerceVector( x, INTSXP) ;
            default:
                throw ::Rcpp::not_compatible( "not compatible with INTSXP" ) ;
            }
            return R_NilValue ; /* -Wall */
        }

        template<> SEXP r_true_cast<REALSXP>( SEXP x) {
            switch( TYPEOF( x ) ){
            case INTSXP:
            case LGLSXP:
            case CPLXSXP:
            case RAWSXP:
                return Rf_coerceVector( x, REALSXP );
            default:
                throw ::Rcpp::not_compatible( "not compatible with REALSXP" ) ;
            }
            return R_NilValue ; /* -Wall */
        }

        template<> SEXP r_true_cast<LGLSXP>( SEXP x) {
            switch( TYPEOF( x ) ){
            case REALSXP:
            case INTSXP:
            case CPLXSXP:
            case RAWSXP:
                return Rf_coerceVector( x, LGLSXP );
            default:
                throw ::Rcpp::not_compatible( "not compatible with LGLSXP" ) ;
            }
            return R_NilValue ; /* -Wall */
        }

        template<> SEXP r_true_cast<RAWSXP>( SEXP x) {
            switch( TYPEOF( x ) ){
            case LGLSXP:
            case REALSXP:
            case INTSXP:
            case CPLXSXP:
                return Rf_coerceVector( x, RAWSXP );
            default:
                throw ::Rcpp::not_compatible( "not compatible with RAWSXP" ) ;
            }
            return R_NilValue ; /* -Wall */
        }


        template<> SEXP r_true_cast<CPLXSXP>( SEXP x) {
            switch( TYPEOF( x ) ){
            case RAWSXP:
            case LGLSXP:
            case REALSXP:
            case INTSXP:
                return Rf_coerceVector( x, CPLXSXP );
            default:
                throw ::Rcpp::not_compatible( "not compatible with CPLXSXP" ) ;
            }
            return R_NilValue ; /* -Wall */
        }

        template<> SEXP r_true_cast<STRSXP>( SEXP x) {
            switch( TYPEOF( x ) ){
            case CPLXSXP:
            case RAWSXP:
            case LGLSXP:
            case REALSXP:
            case INTSXP:
                {
                    // return Rf_coerceVector( x, STRSXP );
                    // coerceVector does not work for some reason
                    SEXP call = PROTECT( Rf_lang2( Rf_install( "as.character" ), x ) ) ;
                    SEXP res  = PROTECT( Rf_eval( call, R_GlobalEnv ) ) ;
                    UNPROTECT(2); 
                    return res ;
                }
            case CHARSXP:
                return Rf_ScalarString( x ) ;
            case SYMSXP:
                return Rf_ScalarString( PRINTNAME( x ) ) ; 
            default:
                throw ::Rcpp::not_compatible( "not compatible with STRSXP" ) ;
            }
            return R_NilValue ; /* -Wall */
        }

        template<> SEXP r_true_cast<VECSXP>(SEXP x) {
            return convert_using_rfunction(x, "as.list" ) ;
        }
    
        template<> SEXP r_true_cast<EXPRSXP>(SEXP x) {
            return convert_using_rfunction(x, "as.expression" ) ;
        }

        template<> SEXP r_true_cast<LISTSXP>(SEXP x) {
            switch( TYPEOF(x) ){
            case LANGSXP:
                {
                    SEXP y = R_NilValue ;
                    PROTECT(y = Rf_duplicate( x )); 
                    SET_TYPEOF(y,LISTSXP) ;
                    UNPROTECT(1);
                    return y ;
                }
            default:
                return convert_using_rfunction(x, "as.pairlist" ) ;
            }
        
        }

        template<> SEXP r_true_cast<LANGSXP>(SEXP x) {
            return convert_using_rfunction(x, "as.call" ) ;
        }
    }
}
// }}}

// {{{ random number generators

namespace Rcpp{
    namespace internal{
        namespace {
            unsigned long RNGScopeCounter = 0;
        }
        
        void enterRNGScope() {       
            if (RNGScopeCounter == 0)
                GetRNGstate();       
            RNGScopeCounter++;
        }
        
        void exitRNGScope() {
            RNGScopeCounter--;
            if (RNGScopeCounter == 0)
                PutRNGstate();
        }
    } // internal
	
}
// }}}

namespace Rcpp{
    SEXP Rcpp_eval(SEXP expr, SEXP env) {
        PROTECT(expr);
        
        reset_current_error() ; 
        
        Environment RCPP = Environment::Rcpp_namespace(); 
        static SEXP tryCatchSym = NULL, evalqSym, conditionMessageSym, errorRecorderSym, errorSym ;
        if (!tryCatchSym) {
            tryCatchSym               = ::Rf_install("tryCatch");
            evalqSym                  = ::Rf_install("evalq");
            conditionMessageSym       = ::Rf_install("conditionMessage");
            errorRecorderSym          = ::Rf_install(".rcpp_error_recorder");
            errorSym                  = ::Rf_install("error");
        }
        
        SEXP call = PROTECT( Rf_lang3( 
            tryCatchSym, 
            Rf_lang3( evalqSym, expr, env ),
            errorRecorderSym
        ) ) ;
        SET_TAG( CDDR(call), errorSym ) ;
        /* call the tryCatch call */
        SEXP res  = PROTECT(::Rf_eval( call, RCPP ) );
        
        UNPROTECT(3) ;
        
        if( error_occured() ) {
            SEXP current_error        = PROTECT( rcpp_get_current_error() ) ;
            SEXP conditionMessageCall = PROTECT(::Rf_lang2(conditionMessageSym, current_error)) ;
            SEXP condition_message    = PROTECT(::Rf_eval(conditionMessageCall, R_GlobalEnv)) ;
            std::string message(CHAR(::Rf_asChar(condition_message)));
            UNPROTECT( 3 ) ;
            throw eval_error(message) ;
        }
        
        return res ;
    }
}   
