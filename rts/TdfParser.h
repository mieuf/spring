#ifndef TDFPARSER_H_INCLUDED
#define TDFPARSER_H_INCLUDED
//
//////////////////////////////////////////////////////////////////////

#pragma warning(disable:4786)

#include <string>
#include <vector>
#include <map>
#include <sstream>
#include <stdexcept>

class TdfParser  
{
public:
  struct parse_error : public std::runtime_error {
    private:
      std::size_t line, column;
      std::string filename;
    public:
      parse_error( std::size_t line, std::size_t column, std::string const& filename ) throw();
      parse_error( std::string const& line_of_error, std::size_t line, std::size_t column, std::string const& filename ) throw();
      ~parse_error() throw();
      std::size_t get_line() const;
      std::size_t get_column() const;
      std::string const& get_filename() const;
  };
  struct TdfSection
  {
    std::map<std::string, TdfSection*> sections;
    std::map<std::string, std::string> values;
    void print( std::ostream & out ) const;
    void add_name_value( std::string const& name, std::string& value );
    TdfSection* construct_subsection( std::string const& name );

    ~TdfSection();
  };

	TdfParser();
	TdfParser( std::string const& filename );
	TdfParser( const char* buffer, std::size_t size );

  void print( std::ostream & out ) const;

  void LoadFile( std::string const& file );
	void LoadBuffer( const char* buffer, std::size_t size );
	virtual ~TdfParser();


	/**
		*  @param value pointer to string to store the value in.
		*  @param ... location of value, terminate with NULL.
		*  @return true on success.
		
	bool GetValue(std::string &value, ...);*/

	/**
		*  Retreive a specific value from the file and returns it, gives an error messagebox if value not found.
		*  @param location location of value in the form "section\\section\\ ... \\name".
		*  @return returns the value on success, undefined on failure.
		*/	
	std::string SGetValueMSG(std::string const& location);

	/**
		*  Retreive a specific value from the file and returns it, returns the specified default value if not found.
		*  @param defaultvalue
		*  @param location location of value.
		*  @return returns the value on success, default otherwise.
		*/		
	std::string SGetValueDef(std::string const& defaultvalue, std::string const& location);


	/**
		*  Retreive a specific value from the file and returns it.
		*  @param value string to store value in.
		*  @param location location of value in the form "section\\section\\ ... \\name".
		*  @return returns true on success, false otherwise and error message in value.
		*/	
	bool SGetValue(std::string &value, std::string const& location);
	const std::map<std::string, std::string> GetAllValues(std::string const& location);
	std::vector<std::string> GetSectionList(std::string const& location);
	bool SectionExist(std::string const& location);
//	void Test();

	template<typename T>
	void ParseArray(std::string const& value, T *array, int length)
	{
		std::stringstream stream;
		stream << value;
	
		for(int i=0; i<length; i++)
		{
			stream >> array[i];
			//char slask;
			//stream >> slask;

		}
	}

	//template funktion för att häta ett värde, ger errormessagebox om värdet inte fins
	template<typename T>
	void GetMsg(T& value, const std::string& key)
	{
		std::string str;
		str = SGetValueMSG(key);

		std::stringstream stream;
		stream << str;
		stream >> value;
	}

	//template funktion för att häta ett värde, tar defaultvärde om värdet inte fins
	template<typename T>
	void GetDef(T& value, const std::string& defvalue, const std::string& key)
	{
		std::string str;
		str = SGetValueDef(defvalue, key);

		std::stringstream stream;
		stream << str;
		stream >> value;
	}

private:
  TdfSection root_section;
  std::string filename;

	std::vector<std::string> GetLocationVector(std::string const& location);
public:
	float3 GetFloat3(float3 def, std::string const& location);
};

#endif /* SUNPARSER_H */
