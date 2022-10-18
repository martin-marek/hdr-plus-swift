#ifndef ICustomDataHandlerRegistry_h__
#define ICustomDataHandlerRegistry_h__  1

#include "XMPAssetManagement/XMPAssetManagementFwdDeclarations_I.h"

namespace NS_INT_XMPASSETMANAGEMENT {

	using namespace NS_INT_XMPASSETMANAGEMENT;

	class ICustomDataHandlerRegistry {
	public:
		virtual bool IsHandlerRegistered( const std::string & nameSpace, const std::string & name ) const = 0;

		virtual bool RegisterHandler( const std::string & nameSpace, const std::string & name, const spICustomDataHandler & handler ) = 0;

		virtual spICustomDataHandler GetHandler( const std::string & nameSpace, const std::string & name ) = 0;
		virtual spcICustomDataHandler GetHandler( const std::string & nameSpace, const std::string & name ) const = 0;

		static pICustomDataHandlerRegistry GetInstance();
		static pcICustomDataHandlerRegistry GetConstInstance();

		static pICustomDataHandlerRegistry CreateInstance();
		static void DestroyInstance();

		virtual ~ICustomDataHandlerRegistry() = 0;

	};

	inline ICustomDataHandlerRegistry::~ICustomDataHandlerRegistry() { }

}
#endif  // ICustomDataHandlerRegistry_h__
