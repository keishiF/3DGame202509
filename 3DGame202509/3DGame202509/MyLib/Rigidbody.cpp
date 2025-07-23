#include "Rigidbody.h"

Rigidbody::Rigidbody() :
	m_pos(),
	m_dir(),
	m_velo()
{

}

void Rigidbody::Init()
{
	m_pos  = Vector3(0, 0, 0);
	m_dir  = Vector3(0, 0, 0);
	m_velo = Vector3(0, 0, 0);
}

void Rigidbody::SetVelo(Vector3 velo)
{
	m_velo = velo;
	//ˆÚ“®—Ê‚ª‚ ‚Á‚½‚ç
	if (m_velo.SqrLength() > 0)
	{
		//•ûŒüƒxƒNƒgƒ‹‚ğİ’è‚·‚é
		m_dir = m_velo.GetNormalize();
	}
}
