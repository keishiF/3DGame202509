#include "Rigidbody.h"

Rigidbody::Rigidbody() :
	m_pos(),
	m_dir(),
	m_velo()
{

}

void Rigidbody::Init()
{
	m_pos  = Vec3(0, 0, 0);
	m_dir  = Vec3(0, 0, 0);
	m_velo = Vec3(0, 0, 0);
}

void Rigidbody::SetVelo(Vec3 velo)
{
	m_velo = velo;
	//ˆÚ“®—Ê‚ª‚ ‚Á‚½‚ç
	if (m_velo.SqrLength() > 0)
	{
		//•ûŒüƒxƒNƒgƒ‹‚ğİ’è‚·‚é
		m_dir = m_velo.GetNormalize();
	}
}
