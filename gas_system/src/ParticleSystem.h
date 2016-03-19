#pragma once
#include "vector2f.h"
#include <vector>
#include <math.h>
#include <assert.h>

class Particle
{
public:
	void Integrate(float dt) 
	{
		Vector2f tmp = pos;
		pos = pos + (pos - prevPos) + acceleration * dt * dt;
		prevPos = tmp;
	}

	Vector2f pos;
	float radius;
	Vector2f prevPos;
	Vector2f acceleration;
};

class Link
{
public:
	Link() {}

	Link(Particle *p0, Particle *p1, float stiffness)
	{
		this->p0 = p0;
		this->p1 = p1;
		this->stiffness = stiffness;
		this->defLength = (p1->pos - p0->pos).Length();
	}

	void Solve()
	{
		Vector2f delta = p1->pos - p0->pos;
		p0->pos = p0->pos + delta.Norm()*(delta.Length() - defLength)*0.5f*stiffness;
		p1->pos = p1->pos - delta.Norm()*(delta.Length() - defLength)*0.5f*stiffness;
	}

	Particle *p0;
	Particle *p1;
	float defLength;
	float stiffness;
};

class VolumeLink
{
private:
    std::vector<Particle *> volumeParticles;

    float innerVolume;
    float innerPressure;
    float outerPressure;

public:
    VolumeLink(Particle ** particles, size_t particlesCount, float innerPressure, float outerPressure)
    {
        for (size_t i = 0; i < particlesCount; i++) {
            this->volumeParticles.push_back(particles[i]);
        }

        this->innerPressure = innerPressure;
        this->outerPressure = outerPressure;
        this->innerVolume = ComputeVolume();
    }

    float ComputeVolume()
    {
        float volume = 0;

        for(size_t i = 0; i < volumeParticles.size(); i++)
            volume += volumeParticles[(i + 1) % volumeParticles.size()]->pos ^ volumeParticles[i]->pos * 0.5f; //векторное произведение

        return volume;
    }

    float Solve()
    {
        float currVolume = ComputeVolume();
        float pressureDiff = innerPressure * innerVolume / currVolume - outerPressure;

        for (size_t i = 0; i < volumeParticles.size(); i++) {
            Vector2f delta = volumeParticles[(i + 1) % volumeParticles.size()]->pos - volumeParticles[i]->pos;
            Vector2f force = -1.0f * Vector2f(-delta.y, delta.x) * pressureDiff; //перпендикуляр к ребру, помноженный на разницу давлений
            volumeParticles[(i + 1) % volumeParticles.size()]->pos = volumeParticles[(i + 1) % volumeParticles.size()]->pos + force;
            volumeParticles[i]->pos = volumeParticles[i]->pos + force;
        }
    }
};

class ParticleSystem
{
public:
    ParticleSystem(Vector2f minPoint, Vector2f maxPoint, float innerPressure, float outerPressure)
	{
        this->MinPoint = minPoint;
        this->MaxPoint = maxPoint;
        this->innerPressure = innerPressure;
        this->outerPressure = outerPressure;
    }

    ~ParticleSystem()
    {
        for (size_t i = 0; i < particles.size(); i++) {
            delete particles[i];
        }
    }

    Vector2f getMinPoint()
    {
        return MinPoint;
    }

    Vector2f getMaxPoint()
    {
        return MaxPoint;
    }

    Particle * addParticle(Vector2f pos, float radius)
    {
        Particle * newbie = new Particle();

        newbie->pos = pos;
        newbie->prevPos = pos;
        newbie->acceleration = Vector2f(0.0, 20.0f);
        newbie->radius = radius;
        particles.push_back(newbie);

        return particles.back();
    }

    void addLink(Particle * firstParticle, Particle * secondParticle, float stiffness)
    {
        assert(firstParticle);
        assert(secondParticle);

        links.push_back(Link(firstParticle, secondParticle, stiffness));
    }

    void AddVolumeLink(Particle **particles, int count)
    {
        volumeLinks.push_back(VolumeLink(particles, count, this->innerPressure, this->outerPressure));
    }

	void Update()
	{
        float upperLimit = MaxPoint.y;
        float downLimit = MinPoint.y;
        float leftLimit = MinPoint.x + 50.f;
        float rightLimit = MaxPoint.x;

		for (int i = 0; i < particles.size(); i++)
		{
            particles[i]->Integrate(1e-2f);

            if (particles[i]->pos.y + particles[i]->radius > upperLimit)
			{
                particles[i]->pos.y = upperLimit - particles[i]->radius;
                particles[i]->prevPos.x = particles[i]->pos.x;
			}

            if (particles[i]->pos.y + particles[i]->radius < downLimit)
            {
                particles[i]->pos.y = downLimit + particles[i]->radius;
                particles[i]->prevPos.x = particles[i]->pos.x;
            }

            if (particles[i]->pos.x + particles[i]->radius > rightLimit)
            {
                particles[i]->pos.x = rightLimit - particles[i]->radius;
                particles[i]->prevPos.y = particles[i]->pos.y;
            }

            if (particles[i]->pos.x + particles[i]->radius < leftLimit)
            {
                particles[i]->pos.x = leftLimit + particles[i]->radius;
                particles[i]->prevPos.y = particles[i]->pos.y;
            }
		}

        for (size_t i = 0; i < volumeLinks.size(); i++)
        {
            volumeLinks[i].Solve();
        }

        for (size_t i = 0; i < links.size(); i++)
		{
			links[i].Solve();
		}
	}

    size_t GetParticlesCount()
    {
        return particles.size();
    }

    Particle * getParticle(size_t i)
    {
        return particles[i];
    }

    void Push_Right(Vector2f a)
    {
        for (int i = 0; i < particles.size(); i++)
        {
            particles[i]->pos.x += a.x;
        }
    }

    void Push_Left(Vector2f a)
    {
        for (int i = 0; i < particles.size(); i++)
        {
            particles[i]->pos.x += a.x;
        }
    }

    void Push_Up(Vector2f a)
    {
        for (int i = 0; i < particles.size(); i++)
			{
                particles[i]->pos.y -= a.y;
			}
    }

private:

    std::vector<Particle *> particles;
	std::vector<Link> links;
    std::vector<VolumeLink> volumeLinks;

	Vector2f MinPoint;
	Vector2f MaxPoint; 

    float innerPressure;
    float outerPressure;
};
