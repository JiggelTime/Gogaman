#include <Gogaman.h>

class Sandbox : public Gogaman::Application
{
public:
	Sandbox()
	{
	}

	~Sandbox()
	{
	}
};

Gogaman::Application *Gogaman::CreateApplication()
{
	return new Sandbox();
}