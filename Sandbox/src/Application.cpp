#include <Gogaman.h>
#include <iostream>

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