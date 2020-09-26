#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

long ABS(long x) { return x >= 0 ? x : -x; }

typedef struct
{
	long x, y, z;
} vec3;

typedef struct
{
	vec3 p, v, a;
} particle;

bool vec3_equals(const vec3 *a, const vec3 *b)
{
	return a->x == b->x 
		&& a->y == b->y 
		&& a->z == b->z;
}

vec3 vec3_add(const vec3 *a, const vec3 *b)
{
	vec3 c = {.x = a->x + b->x, .y = a->y + b->y, .z = a->z + b->z};
	return c;
}

long vec3_size(const vec3 *v)
{
	return ABS(v->x) + ABS(v->y) + ABS(v->z);
}

void vec3_print(const char *s, const vec3 *v)
{
	printf("%s [%ld, %ld, %ld]\n", s, v->x, v->y, v->z);
}

void step(particle **ps, int p_size)
{
	for (int i = 0; i < p_size; i++)
	{
		particle *p = ps[i];
		if (p)
		{
			p->v = vec3_add(&p->v, &p->a);
			p->p = vec3_add(&p->p, &p->v);
		}
	}

	

	for (int i = 0; i < p_size; i++)
	{
		if (!ps[i])
			continue;
		bool delete = false;

		for (int j = i + 1; j < p_size; j++)
		{
			if (ps[j] && vec3_equals(&ps[i]->p, &ps[j]->p))
			{
				delete = true;
				free(ps[j]);
				ps[j] = NULL;
			}
		}
		if (delete)
		{
			free(ps[i]);
			ps[i] = NULL;
		}
	}
}

int count(particle **ps, int p_size)
{
	int cnt = 0;
	for (int i = 0; i < p_size; i++) if (ps[i]) cnt++;
	return cnt;
}

int main()
{
	int particles_size = 0;
	int particles_capacity = 10;
	particle **particles = malloc(sizeof(particle*) * particles_capacity);

	FILE *f = fopen("input", "r");
	int x = 0;
	while (!feof(f))
	{
		if (particles_size == particles_capacity)
			particles = realloc(particles, sizeof(particle*) * (particles_capacity *= 2));
		particle *p = particles[particles_size++] = malloc(sizeof(particle));

		int r = fscanf(f, "p=<%ld,%ld,%ld>, v=<%ld,%ld,%ld>, a=<%ld,%ld,%ld>\n"
				, &p->p.x, &p->p.y, &p->p.z
				, &p->v.x, &p->v.y, &p->v.z
				, &p->a.x, &p->a.y, &p->a.z);
	}
	fclose(f);

	printf("%d particles read\n", particles_size);
		/*for (int j = 0; j < particles_size; j++)
		{
			char buf[80];
			sprintf(buf, "P_%d", j);
			vec3_print(buf, &particles[j].p);
			printf("Dist: %d\n", vec3_size(&particles[j].p));
		}*/

	for (int i = 0; i < 100 ; i++)
	{
		step(particles, particles_size);
		long d;
	}


	int cnt = count(particles, particles_size);
	printf("Part 2: %d\n", cnt);

	// cleanup
	for (int i = 0; i < particles_size; i++)
		if (particles[i])
		{
			free(particles[i]);
			particles[i] = NULL;
		}
	free(particles);
	particles = NULL;
	
	printf("done\n");
	return 0;
}
