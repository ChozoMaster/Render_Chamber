// Vamco:
// Author: Dennis Basgier
// 20.01.2013
#include "MouseInteractorRubberBand.h"
#include "vtkCamera.h"

#define VTKISRBP_ORIENT 0
#define VTKISRBP_SELECT 1

MouseInteractorRubberBand::MouseInteractorRubberBand()
{
	this->CurrentMode = VTKISRBP_ORIENT;
	this->StartPosition[0] = this->StartPosition[1] = 0;
	this->EndPosition[0] = this->EndPosition[1] = 0;
	this->Moving = 0;
	this->PixelArray = vtkUnsignedCharArray::New();
}


void MouseInteractorRubberBand::StartSelect()
{
	this->CurrentMode = VTKISRBP_SELECT;
}

void MouseInteractorRubberBand::OnChar()
{
	switch (this->Interactor->GetKeyCode())
	{
	case 'r':
	case 'R':
		this->Superclass::Superclass::OnChar();
		break;

	case 's':
	case 'S':
		//r toggles the rubber band selection mode for mouse button 1
		if (this->CurrentMode == VTKISRBP_ORIENT)
		{
			this->CurrentMode = VTKISRBP_SELECT;
		}
		else
		{ 
			this->CurrentMode = VTKISRBP_ORIENT;
		}
		break;

	case 'p' :
	case 'P' :
		{
			vtkRenderWindowInteractor *rwi = this->Interactor;
			int *eventPos = rwi->GetEventPosition();
			this->FindPokedRenderer(eventPos[0], eventPos[1]);
			this->StartPosition[0] = eventPos[0];
			this->StartPosition[1] = eventPos[1];
			this->EndPosition[0] = eventPos[0];
			this->EndPosition[1] = eventPos[1];
			this->Pick();
			break;
		}
	default:
		this->Superclass::OnChar();
	}
}

void MouseInteractorRubberBand::OnLeftButtonDown()
{
	if (this->CurrentMode == 3)
	{	
		if (!this->Interactor)
		{
			return;
		}
		this->Moving = 1;
		vtkInteractorStyleTrackballActor::OnLeftButtonDown();
		return;
	}

	if (this->CurrentMode != VTKISRBP_SELECT)
	{
		//if not in rubber band mode, let the parent class handle it
		//this->Superclass::OnLeftButtonDown();


		if (!this->Interactor)
		{
			return;
		}
		this->Moving = 1;

		vtkRenderWindow *renWin = this->Interactor->GetRenderWindow();

		this->StartPosition[0] = this->Interactor->GetEventPosition()[0];
		this->StartPosition[1] = this->Interactor->GetEventPosition()[1];
		this->EndPosition[0] = this->StartPosition[0];
		this->EndPosition[1] = this->StartPosition[1];

		this->PixelArray->Initialize();
		this->PixelArray->SetNumberOfComponents(3);
		int *size = renWin->GetSize();
		this->PixelArray->SetNumberOfTuples(size[0]*size[1]);

		renWin->GetPixelData(0, 0, size[0]-1, size[1]-1, 1, this->PixelArray);

		this->FindPokedRenderer(this->StartPosition[0], this->StartPosition[1]);



		return;
	}

	if (!this->Interactor)
	{
		return;
	}

	//otherwise record the rubber band starting coordinate

	this->Moving = 1;

	vtkRenderWindow *renWin = this->Interactor->GetRenderWindow();

	this->StartPosition[0] = this->Interactor->GetEventPosition()[0];
	this->StartPosition[1] = this->Interactor->GetEventPosition()[1];
	this->EndPosition[0] = this->StartPosition[0];
	this->EndPosition[1] = this->StartPosition[1];

	this->PixelArray->Initialize();
	this->PixelArray->SetNumberOfComponents(4);
	int *size = renWin->GetSize();
	this->PixelArray->SetNumberOfTuples(size[0]*size[1]);

	renWin->GetRGBACharPixelData(0, 0, size[0]-1, size[1]-1, 1, this->PixelArray);

	this->FindPokedRenderer(this->StartPosition[0], this->StartPosition[1]);
}

void MouseInteractorRubberBand::Zoom()
{
	int width, height;
	width = abs(this->EndPosition[0] - this->StartPosition[0]);
	height = abs(this->EndPosition[1] - this->StartPosition[1]);
	int *size = this->CurrentRenderer->GetSize();
	int *origin = this->CurrentRenderer->GetOrigin();
	vtkCamera *cam = this->CurrentRenderer->GetActiveCamera();

	int min[2];
	double rbcenter[3];
	min[0] = this->StartPosition[0] < this->EndPosition[0] ?
		this->StartPosition[0] : this->EndPosition[0];
	min[1] = this->StartPosition[1] < this->EndPosition[1] ?
		this->StartPosition[1] : this->EndPosition[1];

	rbcenter[0] = min[0] + 0.5*width;
	rbcenter[1] = min[1] + 0.5*height;
	rbcenter[2] = 0;

	this->CurrentRenderer->SetDisplayPoint(rbcenter);
	this->CurrentRenderer->DisplayToView();
	this->CurrentRenderer->ViewToWorld();

	double invw;
	double worldRBCenter[4];
	this->CurrentRenderer->GetWorldPoint(worldRBCenter);
	invw = 1.0/worldRBCenter[3];
	worldRBCenter[0] *= invw;
	worldRBCenter[1] *= invw;
	worldRBCenter[2] *= invw;

	double winCenter[3];
	winCenter[0] = origin[0] + 0.5*size[0];
	winCenter[1] = origin[1] + 0.5*size[1];
	winCenter[2] = 0;

	this->CurrentRenderer->SetDisplayPoint(winCenter);
	this->CurrentRenderer->DisplayToView();
	this->CurrentRenderer->ViewToWorld();

	double worldWinCenter[4];
	this->CurrentRenderer->GetWorldPoint(worldWinCenter);
	invw = 1.0/worldWinCenter[3];
	worldWinCenter[0] *= invw;
	worldWinCenter[1] *= invw;
	worldWinCenter[2] *= invw;

	double translation[3];
	translation[0] = worldRBCenter[0] - worldWinCenter[0];
	translation[1] = worldRBCenter[1] - worldWinCenter[1];
	translation[2] = worldRBCenter[2] - worldWinCenter[2];

	double pos[3], fp[3];
	cam->GetPosition(pos);
	cam->GetFocalPoint(fp);

	pos[0] += translation[0]; pos[1] += translation[1]; pos[2] += translation[2];
	fp[0] += translation[0];  fp[1] += translation[1];  fp[2] += translation[2];

	cam->SetPosition(pos);
	cam->SetFocalPoint(fp);

	double zoomFactor;
	if (width > height)
	{
		zoomFactor = size[0] / static_cast<double>(width);
	}
	else
	{
		zoomFactor = size[1] / static_cast<double>(height);
	}
	if (cam->GetParallelProjection())
	{
		cam->Zoom(zoomFactor);
	}
	else
	{
		// In perspective mode, zoom in by moving the camera closer.  Because we are
		// moving the camera closer, we have to be careful to try to adjust the
		// clipping planes to best match the actual position they were in before.
		double initialDistance = cam->GetDistance();
		cam->Dolly(zoomFactor);
		double finalDistance = cam->GetDistance();
		double deltaDistance = initialDistance - finalDistance;
		double clippingRange[2];
		cam->GetClippingRange(clippingRange);
		clippingRange[0] -= deltaDistance;
		clippingRange[1] -= deltaDistance;
		// Correct bringing clipping planes too close or behind camera.
		if (clippingRange[1] <= 0.0)
		{
			clippingRange[1] = 0.001;
		}
		// This near plane check comes from vtkRenderer::ResetCameraClippingRange()
		if (clippingRange[0] < 0.001*clippingRange[1])
		{
			clippingRange[0] = 0.001*clippingRange[1];
		}
		cam->SetClippingRange(clippingRange);
	}

	this->Interactor->Render();
}


void MouseInteractorRubberBand::OnMouseMove()
{
	if (this->CurrentMode == 3)
	{
		vtkInteractorStyleTrackballActor::OnMouseMove();
		return;
	}
	else{
		if (this->CurrentMode != VTKISRBP_SELECT)
		{
			//if not in rubber band mode,  let the parent class handle it
			//this->Superclass::OnMouseMove();


			if (!this->Interactor || !this->Moving)
			{
				return;
			}

			this->EndPosition[0] = this->Interactor->GetEventPosition()[0];
			this->EndPosition[1] = this->Interactor->GetEventPosition()[1];  
			int *size = this->Interactor->GetRenderWindow()->GetSize();  
			if (this->EndPosition[0] > (size[0]-1))
			{
				this->EndPosition[0] = size[0]-1;
			}
			if (this->EndPosition[0] < 0)
			{
				this->EndPosition[0] = 0;
			}
			if (this->EndPosition[1] > (size[1]-1))
			{
				this->EndPosition[1] = size[1]-1;
			}
			if (this->EndPosition[1] < 0)
			{
				this->EndPosition[1] = 0;
			}

			vtkUnsignedCharArray *tmpPixelArray = vtkUnsignedCharArray::New();
			tmpPixelArray->DeepCopy(this->PixelArray);

			unsigned char *pixels = tmpPixelArray->GetPointer(0);

			int min[2], max[2];
			min[0] = this->StartPosition[0] <= this->EndPosition[0] ?
				this->StartPosition[0] : this->EndPosition[0];
			min[1] = this->StartPosition[1] <= this->EndPosition[1] ?
				this->StartPosition[1] : this->EndPosition[1];
			max[0] = this->EndPosition[0] > this->StartPosition[0] ?
				this->EndPosition[0] : this->StartPosition[0];
			max[1] = this->EndPosition[1] > this->StartPosition[1] ?
				this->EndPosition[1] : this->StartPosition[1];

			int i;
			for (i = min[0]; i <= max[0]; i++)
			{
				pixels[3*(min[1]*size[0]+i)] = 255 ^ pixels[3*(min[1]*size[0]+i)];
				pixels[3*(min[1]*size[0]+i)+1] = 255 ^ pixels[3*(min[1]*size[0]+i)+1];
				pixels[3*(min[1]*size[0]+i)+2] = 255 ^ pixels[3*(min[1]*size[0]+i)+2];
				pixels[3*(max[1]*size[0]+i)] = 255 ^ pixels[3*(max[1]*size[0]+i)];
				pixels[3*(max[1]*size[0]+i)+1] = 255 ^ pixels[3*(max[1]*size[0]+i)+1];
				pixels[3*(max[1]*size[0]+i)+2] = 255 ^ pixels[3*(max[1]*size[0]+i)+2];
			}
			for (i = min[1]+1; i < max[1]; i++)
			{
				pixels[3*(i*size[0]+min[0])] = 255 ^ pixels[3*(i*size[0]+min[0])];
				pixels[3*(i*size[0]+min[0])+1] = 255 ^ pixels[3*(i*size[0]+min[0])+1];
				pixels[3*(i*size[0]+min[0])+2] = 255 ^ pixels[3*(i*size[0]+min[0])+2];
				pixels[3*(i*size[0]+max[0])] = 255 ^ pixels[3*(i*size[0]+max[0])];
				pixels[3*(i*size[0]+max[0])+1] = 255 ^ pixels[3*(i*size[0]+max[0])+1];
				pixels[3*(i*size[0]+max[0])+2] = 255 ^ pixels[3*(i*size[0]+max[0])+2];
			}

			this->Interactor->GetRenderWindow()->SetPixelData(0, 0, size[0]-1, size[1]-1, pixels, 1);

			tmpPixelArray->Delete();



			return;
		}


		if (!this->Interactor || !this->Moving)
		{
			return;
		}

		this->EndPosition[0] = this->Interactor->GetEventPosition()[0];
		this->EndPosition[1] = this->Interactor->GetEventPosition()[1];  
		int *size = this->Interactor->GetRenderWindow()->GetSize();  
		if (this->EndPosition[0] > (size[0]-1))
		{
			this->EndPosition[0] = size[0]-1;
		}
		if (this->EndPosition[0] < 0)
		{
			this->EndPosition[0] = 0;
		}
		if (this->EndPosition[1] > (size[1]-1))
		{
			this->EndPosition[1] = size[1]-1;
		}
		if (this->EndPosition[1] < 0)
		{
			this->EndPosition[1] = 0;
		}
		this->RedrawRubberBand();
	}
}




void MouseInteractorRubberBand::OnLeftButtonUp()
{
	if (this->CurrentMode == 3)
	{
		this->CurrentMode == 0;
		//vtkInteractorStyleTrackballActor::OnLeftButtonUp();
		return;
	}
	else {
		if (this->CurrentMode != VTKISRBP_SELECT)
		{
			//if not in rubber band mode,  let the parent class handle it
			//this->Superclass::OnLeftButtonUp();


			if (!this->Interactor || !this->Moving)
			{
				return;
			}

			if (   (this->StartPosition[0] != this->EndPosition[0])
				|| (this->StartPosition[1] != this->EndPosition[1]) )
			{
				this->Zoom();
			}
			this->Moving = 0;



			return;
		}



		if (!this->Interactor || !this->Moving)
		{
			return;
		}

		//otherwise record the rubber band end coordinate and then fire off a pick
		if (   (this->StartPosition[0] != this->EndPosition[0])
			|| (this->StartPosition[1] != this->EndPosition[1]) )
		{
			this->Pick();
		}
		this->Moving = 0;
		//this->CurrentMode = VTKISRBP_ORIENT;
	}
}
void MouseInteractorRubberBand::RedrawRubberBand()
{
	//update the rubber band on the screen
	int *size = this->Interactor->GetRenderWindow()->GetSize();  

	vtkUnsignedCharArray *tmpPixelArray = vtkUnsignedCharArray::New();
	tmpPixelArray->DeepCopy(this->PixelArray);  
	unsigned char *pixels = tmpPixelArray->GetPointer(0);

	int min[2], max[2];

	min[0] = this->StartPosition[0] <= this->EndPosition[0] ?
		this->StartPosition[0] : this->EndPosition[0];
	if (min[0] < 0) { min[0] = 0; }
	if (min[0] >= size[0]) { min[0] = size[0] - 1; }

	min[1] = this->StartPosition[1] <= this->EndPosition[1] ?
		this->StartPosition[1] : this->EndPosition[1];
	if (min[1] < 0) { min[1] = 0; }
	if (min[1] >= size[1]) { min[1] = size[1] - 1; }

	max[0] = this->EndPosition[0] > this->StartPosition[0] ?
		this->EndPosition[0] : this->StartPosition[0];
	if (max[0] < 0) { max[0] = 0; }
	if (max[0] >= size[0]) { max[0] = size[0] - 1; }

	max[1] = this->EndPosition[1] > this->StartPosition[1] ?
		this->EndPosition[1] : this->StartPosition[1];
	if (max[1] < 0) { max[1] = 0; }
	if (max[1] >= size[1]) { max[1] = size[1] - 1; }

	int i;
	for (i = min[0]; i <= max[0]; i++)
	{
		pixels[4*(min[1]*size[0]+i)] = 255 ^ pixels[4*(min[1]*size[0]+i)];
		pixels[4*(min[1]*size[0]+i)+1] = 255 ^ pixels[4*(min[1]*size[0]+i)+1];
		pixels[4*(min[1]*size[0]+i)+2] = 255 ^ pixels[4*(min[1]*size[0]+i)+2];
		pixels[4*(max[1]*size[0]+i)] = 255 ^ pixels[4*(max[1]*size[0]+i)];
		pixels[4*(max[1]*size[0]+i)+1] = 255 ^ pixels[4*(max[1]*size[0]+i)+1];
		pixels[4*(max[1]*size[0]+i)+2] = 255 ^ pixels[4*(max[1]*size[0]+i)+2];
	}
	for (i = min[1]+1; i < max[1]; i++)
	{
		pixels[4*(i*size[0]+min[0])] = 255 ^ pixels[4*(i*size[0]+min[0])];
		pixels[4*(i*size[0]+min[0])+1] = 255 ^ pixels[4*(i*size[0]+min[0])+1];
		pixels[4*(i*size[0]+min[0])+2] = 255 ^ pixels[4*(i*size[0]+min[0])+2];
		pixels[4*(i*size[0]+max[0])] = 255 ^ pixels[4*(i*size[0]+max[0])];
		pixels[4*(i*size[0]+max[0])+1] = 255 ^ pixels[4*(i*size[0]+max[0])+1];
		pixels[4*(i*size[0]+max[0])+2] = 255 ^ pixels[4*(i*size[0]+max[0])+2];
	}

	this->Interactor->GetRenderWindow()->SetRGBACharPixelData(0, 0, size[0]-1, size[1]-1, pixels, 0);
	this->Interactor->GetRenderWindow()->Frame();

	tmpPixelArray->Delete();
}


void MouseInteractorRubberBand::Pick()
{
	//find rubber band lower left, upper right and center  
	double rbcenter[3];
	int *size = this->Interactor->GetRenderWindow()->GetSize();  
	int min[2], max[2];
	min[0] = this->StartPosition[0] <= this->EndPosition[0] ?
		this->StartPosition[0] : this->EndPosition[0];
	if (min[0] < 0) { min[0] = 0; }
	if (min[0] >= size[0]) { min[0] = size[0] - 2; }

	min[1] = this->StartPosition[1] <= this->EndPosition[1] ?
		this->StartPosition[1] : this->EndPosition[1];
	if (min[1] < 0) { min[1] = 0; }
	if (min[1] >= size[1]) { min[1] = size[1] - 2; }

	max[0] = this->EndPosition[0] > this->StartPosition[0] ?
		this->EndPosition[0] : this->StartPosition[0];
	if (max[0] < 0) { max[0] = 0; }
	if (max[0] >= size[0]) { max[0] = size[0] - 2; }

	max[1] = this->EndPosition[1] > this->StartPosition[1] ?
		this->EndPosition[1] : this->StartPosition[1];
	if (max[1] < 0) { max[1] = 0; }
	if (max[1] >= size[1]) { max[1] = size[1] - 2; }

	rbcenter[0] = (min[0] + max[0])/2.0;
	rbcenter[1] = (min[1] + max[1])/2.0;
	rbcenter[2] = 0;

	if (this->State == VTKIS_NONE) 
	{
		//tell the RenderWindowInteractor's picker to make it happen
		vtkRenderWindowInteractor *rwi = this->Interactor;

		vtkAssemblyPath *path = NULL;
		rwi->StartPickCallback();
		vtkAbstractPropPicker *picker = 
			vtkAbstractPropPicker::SafeDownCast(rwi->GetPicker());
		if ( picker != NULL )
		{
			vtkAreaPicker *areaPicker = vtkAreaPicker::SafeDownCast(picker);
			if (areaPicker != NULL)
			{
				areaPicker->AreaPick(min[0], min[1], max[0], max[1], 
					this->CurrentRenderer);
			}
			else
			{
				picker->Pick(rbcenter[0], rbcenter[1], 
					0.0, this->CurrentRenderer);
			}
			path = picker->GetPath();
		}
		if ( path == NULL )
		{
			this->HighlightProp(NULL);
			this->PropPicked = 0;
		}
		else
		{
			//highlight the one prop that the picker saved in the path
			//this->HighlightProp(path->GetFirstNode()->GetViewProp());
			this->PropPicked = 1;
		}
		rwi->EndPickCallback();
	}

	this->Interactor->Render();
}

void MouseInteractorRubberBand::OnMiddleButtonDown()
{
	this->CurrentMode = 3;
	OnLeftButtonDown();
}

void MouseInteractorRubberBand::OnMiddleButtonUp()
{
	this->CurrentMode = 0;
	this->Moving = 0;
}

