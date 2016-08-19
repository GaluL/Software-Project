/*
 * SPPoint.c
 *
 *  Created on: May 15, 2016
 *      Author: galkl
 */
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include "SPPoint.h"

typedef struct sp_point_t
{
	double* coords;
	int dim;
	int index;
} POINT_STRUCT;


SPPoint spPointCreate(double* data, int dim, int index)
{
	SPPoint point = NULL;
	int i = 0;
// checks for basic case
	if (!data || dim <= 0 || index < 0)
	{
		return NULL;
	}
// allocating new memory for point
	point = (SPPoint)malloc(sizeof(*point));
// if a memory allocation problem occurred return null
	if (!point)
	{
		return NULL;
	}

// filling the point fields
	point->dim = dim;
	point->index = index;
	point->coords = (double*)malloc(dim * sizeof(double));
	if (!point->coords)
	{
		free(point);
		return NULL;
	}

	for (i = 0; i < dim; i++)
	{
		point->coords[i] = data[i];
	}

	return point;
}

/**
 * Allocates a copy of the given point.
 *
 * Given the point source, the functions returns a
 * new pint P = (P_1,...,P_{dim-1}) such that:
 * - P_i = source_i (The ith coordinate of source and P are the same)
 * - dim(P) = dim(source) (P and source have the same dimension)
 * - index(P) = index(source) (P and source have the same index)
 *
 * @param source - The source point
 * @assert (source != NUlL)
 * @return
 * NULL in case memory allocation occurs
 * Others a copy of source is returned.
 */
SPPoint spPointCopy(SPPoint source)
{
//	creating the copy
	SPPoint copy = NULL;
// checking for basic condition
	assert(source != NULL);
// generating the new copy
	copy = spPointCreate(source->coords, source->dim, source->index);

	return copy;
}

void spPointDestroy(SPPoint point)
{
	if (point)
	{
//		releasing the the point coord data
		if (point->coords)
		{
			free(point->coords);
		}

		free(point);
	}
}


int spPointGetDimension(SPPoint point)
{
	assert(point != NULL);

	return point->dim;
}


int spPointGetIndex(SPPoint point)
{
	assert(point != NULL);

	return point->index;
}

double spPointGetAxisCoor(SPPoint point, int axis)
{
	assert(point != NULL && axis < point->dim);

	return point->coords[axis];
}

/**
 * Calculates the L2-squared distance between p and q.
 * The L2-squared distance is defined as:
 * (p_1 - q_1)^2 + (p_2 - q_1)^2 + ... + (p_dim - q_dim)^2
 *
 * @param p - The first point
 * @param q - The second point
 * @assert p!=NULL AND q!=NULL AND dim(p) == dim(q)
 * @return
 * The L2-Squared distance between p and q
 */
double spPointL2SquaredDistance(SPPoint p, SPPoint q)
{
	double distance = 0;
	int i = 0;
// checking for basic conditions
	assert(p != NULL && q != NULL && p->dim == q->dim);
//	calculating the distance
	for (i = 0; i < p->dim; i++)
	{
		distance += (p->coords[i] - q->coords[i]) * (p->coords[i] - q->coords[i]);
	}

	return distance;
}

