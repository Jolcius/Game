#ifndef RAY_H
#define RAY_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


class Ray
{
public:
	Ray(glm::vec3 ori, glm::vec3 dir)
	{
		origin = ori;
		direction = dir;
	}

	// Ͷ�����ߣ��ж����Χ���Ƿ��ཻ�����ؽ�����ȣ�z��������Ϊ��Χ�е���󶥵����С����
	float RayCast(glm::vec3 high_P, glm::vec3 low_P)
	{
        // ���� t_min �� t_max ��ʼֵ
        float t_min = (low_P.x - origin.x) / direction.x;
        float t_max = (high_P.x - origin.x) / direction.x;

        if (t_min > t_max) std::swap(t_min, t_max);

        float ty_min = (low_P.y - origin.y) / direction.y;
        float ty_max = (high_P.y - origin.y) / direction.y;

        if (ty_min > ty_max) std::swap(ty_min, ty_max);

        // ��� t_min �� t_max �Ƿ��� y ƽ�����ཻ
        if ((t_min > ty_max) || (ty_min > t_max))
            return -1.0f;

        // ���� t_min �� t_max
        if (ty_min > t_min)
            t_min = ty_min;
        if (ty_max < t_max)
            t_max = ty_max;

        float tz_min = (low_P.z - origin.z) / direction.z;
        float tz_max = (high_P.z - origin.z) / direction.z;

        if (tz_min > tz_max) std::swap(tz_min, tz_max);

        // ��� t_min �� t_max �Ƿ��� z ƽ�����ཻ
        if ((t_min > tz_max) || (tz_min > t_max))
            return -1.0f;

        // ���� t_min �� t_max
        if (tz_min > t_min)
            t_min = tz_min;
        if (tz_max < t_max)
            t_max = tz_max;

        // ��� t_min �� t_max ��Ϊ�������������Χ���ཻ
        if (t_min < 0.0f && t_max < 0.0f)
            return -1.0f;

        // ����������ཻ�����
        return t_min < 0.0f ? t_max : t_min;
	}

private:
	glm::vec3 origin;
	glm::vec3 direction;
};

#endif