import constants from './constants'

export const changePage = page => ({
  type: constants.CHANGE_PAGE,
  page,
})

export const changeUser = username => ({
  type: constants.CHANGE_USER,
  username,
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

export const deleteLike = (obiewId, userId) => ({
  type: constants.DELETE_LIKE,
  obiewId,
  userId,
})