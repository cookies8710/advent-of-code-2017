#include <stdio.h>
#include <stdlib.h>

//#define ABS x (x>=0?x:-x)

long ABS(long x) { return x >= 0 ? x : -x; }

typedef struct
{
	long x, y, z;
} vec3;

typedef struct
{
	vec3 p, v, a;
} particle;


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
	printf("%s [%d, %d, %d]\n", s, v->x, v->y, v->z);
}

int closest(const particle *p, int p_size, long *dist)
{
	*dist = 10000000;
	int idx = -1;

	for (int i = 0; i < p_size; i++)
	{
		long d = vec3_size(&p[i].p);
		if (d< *dist)
		{
			*dist = d;
			idx = i;
		}
	}
	return idx;
}
void step(particle *ps, int p_size)
{
	for (int i = 0; i < p_size; i++)
	{
		particle *p = ps + i;
		/*if (i == 110)
		{
			vec3_print("P:", &p->p);
			vec3_print("V:", &p->v);
			vec3_print("A:", &p->a);
			p->v = vec3_add(&p->v, &p->a);
			p->p = vec3_add(&p->p, &p->a);
			printf("after\n");
			vec3_print("P:", &p->p);
			vec3_print("V:", &p->v);
			vec3_print("A:", &p->a);
		}
		else*/
		{
			p->v = vec3_add(&p->v, &p->a);
			p->p = vec3_add(&p->p, &p->a);
		}
	}
}

int main()
{
	int particles_size = 0;
	int particles_capacity = 10;
	particle *particles = malloc(sizeof(particle) * particles_capacity);

	FILE *f = fopen("input", "r");
	int x = 0;
	while (!feof(f))
	{
		if (particles_size == particles_capacity)
			particles = realloc(particles, sizeof(particle) * (particles_capacity *= 2));
		particle *p = particles + particles_size++;

		int r = fscanf(f, "p=<%ld,%ld,%ld>, v=<%ld,%ld,%ld>, a=<%ld,%ld,%ld>\n"
				, &p->p.x, &p->p.y, &p->p.z
				, &p->v.x, &p->v.y, &p->v.z
				, &p->a.x, &p->a.y, &p->a.z);
		
		//printf("%d:", x++);vec3_print("P: ", par);
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

	for (int i = 0; i < 10000; i++)
	{
		step(particles, particles_size);
		long d;
		int c = closest(particles, particles_size, &d);
		printf("[%d] closest: %d (%d)\n", i, c, d);
//		vec3_print("sanity", &particles[110].p);
	}


	// cleanup
	free(particles);
	particles = NULL;
	
	printf("done\n");
	return 0;
}
