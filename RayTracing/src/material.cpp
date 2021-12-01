#include "world.h"
#include "material.h"
#include "vector3D.h"
#include <iostream>

#define epsilon 0.001 // epsilon for numerical precision of reflected ray.
#define max_depth 4 // max recursion depth.

Color Material::reflect(const Ray& incident) const 
{
	Vector3D incidence_dir = incident.getPosition() - incident.getOrigin();
	incidence_dir.normalize();
	Vector3D reflect_origin = incident.getPosition();
	Vector3D reflect_dir = incidence_dir - 2*dotProduct(incidence_dir,incident.getNormal())*incident.getNormal();
	reflect_dir.normalize();
	reflect_origin = reflect_origin + epsilon*reflect_dir;
	Ray reflected_ray(reflect_origin, reflect_dir);
	reflected_ray.setLevel(incident.getLevel()+1);
	return world->shade_ray(reflected_ray);
}

Color Material::refract(const Ray& incident, const int sign) const 
{
	Vector3D incidence_dir = incident.getPosition() - incident.getOrigin();
	incidence_dir.normalize();

	Vector3D incidence_pos = incident.getPosition();
	
	Vector3D refracted_dir;
	float rel_ref_in = eta/incident.getRefractiveIndex();

	if(sign > 0){rel_ref_in = 1/rel_ref_in;} 

	refracted_dir = rel_ref_in*(incidence_dir - dotProduct(incidence_dir,incident.getNormal())*incident.getNormal());
	refracted_dir = refracted_dir - incident.getNormal()*pow(1-pow(rel_ref_in,2)*(1-pow(dotProduct(incidence_dir,incident.getNormal()),2)),0.5);

	Vector3D refract_origin = incidence_pos + epsilon*refracted_dir;
	
	Ray refracted_ray(refract_origin, refracted_dir);
	refracted_ray.setLevel(incident.getLevel()+1);
	return world->shade_ray(refracted_ray);
}

Color Material::shade(const Ray& incident, const bool isSolid) const
{
	Color total_intensity = Color(0);
	Color reflective_intensity = Color(0);
	Color refractive_intensity = Color(0);
	Color tir_intensity = Color(0);
	Color attentuation = Color(1);

	float diff_frac; float specularity;
	float schlik_const = pow((eta-1)/(eta+1),2);

	Vector3D incidence_pos = incident.getPosition();
	int curLevel = incident.getLevel();

	std::vector<LightSource*> lightSources = world->getLightSources();
	for(int i=0; i<lightSources.size(); i++){

		//  Light Source and Direction
		Vector3D l_pos = lightSources[i]->getPosition();
		Color L = lightSources[i]->getIntensity();
		Vector3D l_dir = l_pos-incidence_pos;
		l_dir.normalize();

		// Compute reflection
		if(kr>0 && curLevel<max_depth){
			
			reflective_intensity = reflect(incident);
		}

		// Compute Refraction
		if(kt > 0 && curLevel<max_depth){
			Vector3D incidence_dir = incident.getPosition() - incident.getOrigin();
			incidence_dir.normalize();
			float critical_angle = pow(1-pow(eta/incident.getRefractiveIndex(),2),0.5);
			float rel_ref_ind = eta/incident.getRefractiveIndex();
			float angle = dotProduct(incidence_dir,incident.getNormal());

			float dist = incident.getParameter();
			// Ray enterring dielectric
			if(angle < 0){
				// // TIR
				// if(rel_ref_ind < 1.0 && angle < -critical_angle){
				// 	tir_intensity = reflect(incident);
				// }
				// Refraction
				// else{
				// }
				refractive_intensity = refract(incident, -1);
				schlik_const = schlik_const + (1-schlik_const)*(1-pow(-angle,5));
			}

			// Ray exiting dielectric
			else if(angle > 0){
				attentuation = Color(exp(-ar*dist),exp(-ag*dist),exp(-ab*dist));
				// TIR
				if(rel_ref_ind > 1.0 && angle > critical_angle){
					tir_intensity = reflect(incident);
					// tir_intensity = attentuation*tir_intensity;
					schlik_const = 1.0;
				}
				// Refraction
				else{
					refractive_intensity = refract(incident, 1);
					schlik_const = schlik_const + (1-schlik_const)*(1-pow(angle,5));
					// refractive_intensity = attentuation*refractive_intensity;
				}
			}

		}

		// Create shadows
		Ray shadow_ray(incidence_pos + epsilon*l_dir, l_dir);
		world->firstIntersection(shadow_ray);
		if(shadow_ray.didHit()){continue;}

		// Blinn Phong shading
		incidence_pos.normalize();
		Vector3D halfway = l_dir - incidence_pos;
		halfway.normalize();

		float diff_theta = dotProduct(l_dir,incident.getNormal());
		diff_frac = std::max(0.0f,diff_theta);
		float spec_theta = dotProduct(halfway,incident.getNormal());
		specularity = std::max(0.0f,pow(spec_theta,(float)n));

		total_intensity = total_intensity + kd*color*diff_frac + ks*color*specularity + attentuation*(schlik_const*kr*reflective_intensity + (1-schlik_const)*kt*refractive_intensity + tir_intensity);
	}
	total_intensity = total_intensity + ka*world->getAmbient();
	
	return total_intensity;
}
