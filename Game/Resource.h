#pragma once

namespace Resource
{
	class CResourceBank;

	class CResource
	{
	public:
		CResource( CResourceBank* bank );
		virtual ~CResource();

		virtual void Reload() = 0;
		virtual void Destroy() {};

	private:
		CResourceBank* m_Bank;
	};
}