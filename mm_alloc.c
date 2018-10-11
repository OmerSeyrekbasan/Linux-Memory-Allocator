/*
 * mm_alloc.c
 *
 * Stub implementations of the mm_* routines.
 */

/*
author: ÖMER SEYREKBASAN
OS: LINUX 64 bit
*/

#include "mm_alloc.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

typedef struct METADATA
{
	int size;
	int free; // block kullanılıyorsa 0, kullanılmıyorsa 1
	struct METADATA  *next;
	struct METADATA *prev;
	
}METADATA;


METADATA *head=NULL;



void *mm_malloc(size_t size) {

	METADATA *next;
	METADATA *new_space;
	unsigned char *tmp;
	int i;

	if (size==0)
		return NULL;

	// EGER ALLOCATOR ILK DEFA CAGRRILIYORSA HEAD BELIRLENMELI
	if (head == NULL) {
		head = sbrk(sizeof(METADATA));
		head -> size = 0;
		head -> free = 0;
		head -> next = NULL;
		head -> prev = NULL;
	}
	//YETERI KADAR YER BULUNANA KADAR BLOCKLAR DOLASILMALI
	// SU ANDA KOCAMAN OLSADA HEPSINI ISTEYENE VERIYOR
	next = head;

	if (head->next) {
		next = head -> next;
	}
	while (next -> next != NULL) {
		if (next -> size >= size && next -> free == 1) {
			next -> free = 0;
			//EGER BLOCK COK BUYUKSE BLOCK IKIYE BOLUNUYOR
			if (next -> size >= size + 2*sizeof(METADATA)) {

				new_space = (unsigned char *)next + size + sizeof(METADATA);
				new_space -> size = next -> size - (size+sizeof(METADATA));
				new_space -> free = 1;
				new_space -> next = next -> next;
				if (next -> next)
					next -> next -> prev = new_space;
				new_space -> prev = next;
				next -> next = new_space;
				next -> size= size;		
			}
			tmp = (unsigned char *)next+sizeof(METADATA);

			for (int i = 0; i < size; ++i)
			{	
				tmp[i] = 0;
			}

			return (unsigned char *)next+sizeof(METADATA);
		} 
		next = next -> next;
	}
	//en son kısım kontrol edilmeli cunku while next -> next != null diye kontrol etti
	if (next -> size >= size && next -> free == 1) {
			next -> free = 0;
			//EGER BLOCK COK BUYUKSE BLOCK IKIYE BOLUNUYOR
			if (next -> size >= size + 2*sizeof(METADATA)) {

				new_space = (unsigned char *)next + size + sizeof(METADATA);
				new_space -> size = next -> size - (size+sizeof(METADATA));
				new_space -> free = 1;
				new_space -> next = next -> next;
				if (next -> next)
					next -> next -> prev = new_space;
				new_space -> prev = next;
				next -> next = new_space;
				next -> size= size;		
			}
			tmp = (unsigned char *)next+sizeof(METADATA);

			for (int i = 0; i < size; ++i)
			{	
				tmp[i] = 0;
			}


			return (unsigned char *)next+sizeof(METADATA);
		} 

	// EGER YER BULUNAMADIYSA YENI YER ACILMALI
	new_space = sbrk (sizeof(METADATA)+size);
	if (new_space!=0xffffffffffffffff && new_space!=NULL) {
		//öncelikle header oluşturulur
		new_space -> size = size;
		new_space -> free = 0;
		new_space -> next = NULL;
		new_space -> prev = next;
		next -> next= new_space; 

			tmp = (unsigned char *)new_space + sizeof(METADATA);
			for (int i = 0; i < size; ++i)
			{	
				tmp[i] = 0;
			}
			return (unsigned char *)new_space + sizeof(METADATA);
		}
	return NULL;
}

void *mm_realloc(void *ptr, size_t size) {
    METADATA *old, *new_space,*next;
    unsigned char *tmp;
    int old_size,next_size,i;


    if (!ptr) {
    	ptr = mm_malloc (size);
    	return ptr;
    }

   	if (size == 0) {
   			if (!ptr)
   				return NULL;
   		mm_free(ptr);
   		return NULL;
   	}

    
	//free implemente edildiği için kullanılabilir. ptr kaybedilmediği sürece dataya ulaşılabilir.
	ptr -= sizeof(METADATA);
    old = ptr ;//old metadayı işaret etmelidir.
    ptr += sizeof(METADATA);
    old_size = old -> size; //eski size elde edilir
 

    if (size <= old_size) {
    	printf("HATA\n");
    	return NULL;
    }


    //BONUS
    //öncelikle sonrasındaki blokta yeterince yer var mı diye bakılır.
    //Eger yer cok büyükse ikiye bölünmesi gerekir.
    if (old -> next) {

    	next = old -> next;
	    next_size = next->size;
	  
	   	if (next -> free) {

	   		 if ( next_size + old_size > size + 2*sizeof(METADATA)) {

		    	new_space = (unsigned char *)next + size+ sizeof(METADATA);
				new_space -> size = next_size -(size-old_size); //yeni oluşturulan bloğa tasan kısım hesaplanmıstır.
				new_space -> free = 1;
				new_space -> next = next->next;
				if (next -> next)
					next -> next -> prev = new_space;
				new_space -> prev = old;
				old -> next = new_space;
				old -> size= size;	
				tmp = (unsigned char*)old + sizeof(METADATA)+old_size;

				for (int i = 0; i < size-old_size; ++i) 
				{	
					tmp[i] = 0;
				}

				return (unsigned char *)old + sizeof(METADATA);

		    } else if ( next_size + old_size + sizeof(METADATA) > size) {
		    	old -> size = old_size + next_size + sizeof(METADATA); //Aradaki metadata kaybolacağı için o alanda artık kullanılabilir.
		    	old -> next = next -> next;
		    	tmp = (unsigned char*)old + sizeof(METADATA)+old_size;

				for (int i = 0; i < size-old_size; ++i) 
				{	
					tmp[i] = 0;
				}
		    	return (unsigned char *)old + sizeof(METADATA);
		    }

	   	}
   

    }
   
    //END OF BONUS

    //eger eski blok birlestirilemisse acilabilecek yer var mı diye bakılır.
    new_space = mm_malloc(size);

    //eger yer acilamamıssa null return edilir.
    if (!new_space)
	    return NULL;
	//eger yer acılabilmisse eski alan free edilip içindeki değerler yeni alana taşınmalıdır.
	tmp = (unsigned char*) new_space;

	// yeni açılan yer 0 lanmalıdır

	for (int i = 0; i < size; ++i) 
	{	
		tmp[i] = 0;
	}

	memcpy(new_space,ptr,old_size);

	



	mm_free(ptr);

	return new_space;


}

void mm_free(void *ptr) {
    METADATA *waste;
    METADATA *tmp;
    if (ptr) {
	  	//pointer metadata ya ulasmalıdır
	    ptr = (unsigned char*)ptr - sizeof(METADATA);
	    waste = ptr;
	    waste ->free = 1;
	    //bir alanın bir kez birleştirilmesi yeterlidir çünkü bu boş alanlarda önceden kendi komuşlarıyla birleştirilmiş olmalılar
	    //ardındaki alan free ise birlesmeliler
	    if (waste -> next) {
	    	tmp = waste -> next;
		    if (tmp -> free) {
		    	waste -> next = tmp -> next;
		    	if (tmp -> next)
		    		tmp -> next -> prev = waste;
		    	waste -> size = waste->size + tmp->size + sizeof(METADATA);// METADATALARDAN BIRI AZALIYOR.ARTIK ORASIDA ALLOCATE EDILEBILIR.
		    }
	    }
	    //oncesindeki alan free ise birlesmeliler
	    if (waste ->prev) {
	    	tmp = waste -> prev;
		    if (tmp -> free) {
		    	tmp -> next = waste -> next;
		    	if (waste -> next)
		    		waste -> next ->prev = tmp;
		    	waste->free=0;
		    	tmp -> size += (waste->size + sizeof(METADATA));// METADATALARDAN BIRI AZALIYOR.ARTIK ORASIDA ALLOCATE EDILEBILIR.
		    }
		}
	    
    } 
}