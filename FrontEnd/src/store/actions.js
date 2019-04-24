import constants from './constants'

export const changeUser = userId => ({
  type: constants.CHANGE_USER,
  userId,
})

export const createNewObiew = obiew => ({
  type: constants.CREATE_NEW_OBIEW,
  obiew,
})

export const createNewComment = (obiewId, comment) => ({
  type: constants.CREATE_NEW_COMMENT,
  comment,
  obiewId,
})

export const createNewLike = (obiewId, like) => ({
  type: constants.CREATE_NEW_LIKE,
  obiewId,
  like
})

export const deleteLike = (obiewId, like) => ({
  type: constants.DELETE_LIKE,
  obiewId,
  like,
})