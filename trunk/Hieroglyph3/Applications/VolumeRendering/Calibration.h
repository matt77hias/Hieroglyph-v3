//--------------------------------------------------------------------------------
// Calibration
//
// This class serves as the base class for all calibration object types.
//--------------------------------------------------------------------------------
#ifndef Calibration_h
#define Calibration_h
//--------------------------------------------------------------------------------
#include <string>
//--------------------------------------------------------------------------------
namespace VSI
{
	class Calibration
	{
	public:
		Calibration();
		virtual ~Calibration();

		std::string GetName();
		void SetName( std::string& name );

	protected:
		std::string m_Name;

	};
};
//--------------------------------------------------------------------------------
#endif // Calibration_h
